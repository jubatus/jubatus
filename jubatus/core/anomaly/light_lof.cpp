// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include "light_lof.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include "jubatus/util/data/unordered_map.h"
#include "jubatus/util/lang/bind.h"
#include "../common/exception.hpp"

using jubatus::util::data::unordered_map;
using jubatus::util::data::unordered_set;
using jubatus::util::lang::shared_ptr;
using jubatus::core::nearest_neighbor::nearest_neighbor_base;
using jubatus::core::table::column_table;

namespace jubatus {
namespace core {
namespace anomaly {
namespace {

const uint32_t DEFAULT_NEIGHBOR_NUM = 10;
const uint32_t DEFAULT_REVERSE_NN_NUM = 30;

const size_t KDIST_COLUMN_INDEX = 0;
const size_t LRD_COLUMN_INDEX = 1;

shared_ptr<column_table> create_lof_table() {
  shared_ptr<column_table> table(new column_table);
  std::vector<table::column_type> schema(
      2, table::column_type(table::column_type::float_type));
  table->init(schema);
  return table;
}

float calculate_lof(float lrd, const std::vector<float>& neighbor_lrds) {
  if (neighbor_lrds.empty()) {
    return lrd == 0 ? 1 : std::numeric_limits<float>::infinity();
  }

  const float sum_neighbor_lrd = std::accumulate(
      neighbor_lrds.begin(), neighbor_lrds.end(), 0.0f);

  if (std::isinf(sum_neighbor_lrd) && std::isinf(lrd)) {
    return 1;
  }

  return sum_neighbor_lrd / (neighbor_lrds.size() * lrd);
}

}  // namespace

light_lof::config::config()
    : nearest_neighbor_num(DEFAULT_NEIGHBOR_NUM),
      reverse_nearest_neighbor_num(DEFAULT_REVERSE_NN_NUM) {
}

light_lof::light_lof(
    const config& conf,
    const std::string& id,
    shared_ptr<nearest_neighbor_base> nearest_neighbor_engine)
    : nearest_neighbor_engine_(nearest_neighbor_engine),
      config_(conf),
      my_id_(id) {
  mixable_nearest_neighbor_->set_model(nearest_neighbor_engine_->get_table());
  mixable_scores_->set_model(create_lof_table());
}

light_lof::light_lof(
    const config& conf,
    const std::string& id,
    shared_ptr<nearest_neighbor_base> nearest_neighbor_engine,
    shared_ptr<unlearner::unlearner_base> unlearner)
    : nearest_neighbor_engine_(nearest_neighbor_engine),
      unlearner_(unlearner),
      config_(conf),
      my_id_(id) {
  shared_ptr<column_table> nn_table = nearest_neighbor_engine_->get_table();
  shared_ptr<column_table> lof_table = create_lof_table();
  unlearner_->set_callback(jubatus::util::lang::bind(
      &light_lof::unlearn, this, jubatus::util::lang::_1));

  mixable_nearest_neighbor_->set_model(nn_table);
  mixable_nearest_neighbor_->set_unlearner(unlearner_);
  mixable_scores_->set_model(lof_table);
  mixable_scores_->set_unlearner(unlearner_);
}

light_lof::~light_lof() {
}

float light_lof::calc_anomaly_score(const common::sfv_t& query) const {
  std::vector<float> neighbor_lrds;
  const float lrd = collect_lrds(query, neighbor_lrds);

  return calculate_lof(lrd, neighbor_lrds);
}

float light_lof::calc_anomaly_score(const std::string& id) const {
  std::vector<float> neighbor_lrds;
  const float lrd = collect_lrds(id, neighbor_lrds);

  return calculate_lof(lrd, neighbor_lrds);
}

void light_lof::clear() {
  nearest_neighbor_engine_->clear();
  mixable_scores_->get_model()->clear();
}

void light_lof::clear_row(const std::string& id) {
  throw JUBATUS_EXCEPTION(common::unsupported_method(__func__));
}

void light_lof::update_row(const std::string& id, const sfv_diff_t& diff) {
  throw JUBATUS_EXCEPTION(common::unsupported_method(__func__));
}

void light_lof::set_row(const std::string& id, const common::sfv_t& sfv) {
  unordered_set<uint64_t> update_set;

  shared_ptr<column_table> table = mixable_scores_->get_model();
  if (table->exact_match(id).first) {
    collect_neighbors(id, update_set);
  }

  touch(id);
  nearest_neighbor_engine_->set_row(id, sfv);
  collect_neighbors(id, update_set);

  // Primarily add id to lof table with dummy parameters.
  // update_entries() below overwrites this row.
  table->add(id, table::owner(my_id_), -1.f, -1.f);

  const std::pair<bool, uint64_t> index = table->exact_match(id);
  if (!index.first) {
    throw JUBATUS_EXCEPTION(common::exception::runtime_error(
        "Failed to add a row to lof table (key = " + id + ')'));
  } else {
    update_set.insert(index.second);
  }

  for (unordered_set<uint64_t>::const_iterator it = update_set.begin();
       it != update_set.end(); ++it) {
    touch(table->get_key(*it));
  }

  update_entries(update_set);
}

void light_lof::get_all_row_ids(std::vector<std::string>& ids) const {
  nearest_neighbor_engine_->get_all_row_ids(ids);
}

std::string light_lof::type() const {
  return "light_lof";
}

void light_lof::register_mixables_to_holder(framework::mixable_holder& holder)
    const {
  nearest_neighbor_engine_->register_mixables_to_holder(holder);
  holder.register_mixable(mixable_scores_);
}

// private

void light_lof::touch(const std::string& id) {
  if (unlearner_) {
    unlearner_->touch(id);
  }
}

// Unlearning callback function of light_lof.
//
// It removes a row and updates parameters of its reverse k-nearest neighbors.
// This behavior is similar to ``light_lof::set_row``, which updates reverse
// k-NNs if given id exists already.
//
// It is necessary to update reverse k-NNs of the point to be removed for model
// correctness: if this procedure is omitted, ``light_lof`` no longer runs
// correctly.
void light_lof::unlearn(const std::string& key) {
  unordered_set<uint64_t> reverse_knn;
  collect_neighbors(key, reverse_knn);
    shared_ptr<column_table> table = mixable_scores_->get_model();
  const std::pair<bool, uint64_t> index = table->exact_match(key);

  if (index.first) {
    reverse_knn.erase(index.second);
  } else {
    // TODO(kumagi): assertion failure
  }

  mixable_nearest_neighbor_->get_model()->delete_row(key);
  mixable_scores_->get_model()->delete_row(key);

  update_entries(reverse_knn);
}

float light_lof::collect_lrds(
    const common::sfv_t& query,
    std::vector<float>& neighbor_lrds) const {
  std::vector<std::pair<std::string, float> > neighbors;
  nearest_neighbor_engine_->neighbor_row(
      query, neighbors, config_.nearest_neighbor_num);

  return collect_lrds_from_neighbors(neighbors, neighbor_lrds);
}

float light_lof::collect_lrds(
    const std::string& id,
    std::vector<float>& neighbor_lrds) const {
  std::vector<std::pair<std::string, float> > neighbors;
  nearest_neighbor_engine_->neighbor_row(
      id, neighbors, config_.nearest_neighbor_num + 1);

  // neighbors may contain given id. We ignore it.
  for (size_t i = 0; i < neighbors.size(); ++i) {
    if (neighbors[i].first == id) {
      std::swap(neighbors[i], neighbors.back());
      neighbors.pop_back();
      break;
    }
  }
  if (neighbors.size() > static_cast<size_t>(config_.nearest_neighbor_num)) {
    neighbors.resize(config_.nearest_neighbor_num);
  }

  return collect_lrds_from_neighbors(neighbors, neighbor_lrds);
}

float light_lof::collect_lrds_from_neighbors(
    const std::vector<std::pair<std::string, float> >& neighbors,
    std::vector<float>& neighbor_lrds) const {
  neighbor_lrds.resize(neighbors.size());
  if (neighbors.empty()) {
    return std::numeric_limits<float>::infinity();
  }

  // Collect parameters of given neighbors.
  std::vector<parameter> parameters(neighbors.size());
  for (size_t i = 0; i < neighbors.size(); ++i) {
    parameters[i] = get_row_parameter(neighbors[i].first);
    neighbor_lrds[i] = parameters[i].lrd;
  }

  // Calculate LRD value of the query.
  float sum_reachability = 0;
  for (size_t i = 0; i < neighbors.size(); ++i) {
    // Accumulate the reachability distance of the query and the i-th neighbor.
    sum_reachability += std::max(neighbors[i].second, parameters[i].kdist);
  }

  if (sum_reachability == 0) {
    // All k-nearest neighbors are at the same point with given query.
    return std::numeric_limits<float>::infinity();
  }

  // LRD is an inverse of mean of reachability distances between the query and
  // its k-nearest neighbors.
  return neighbors.size() / sum_reachability;
}

void light_lof::collect_neighbors(
    const std::string& query,
    unordered_set<uint64_t>& neighbors) const {
  std::vector<std::pair<std::string, float> > nn_result;
  nearest_neighbor_engine_->neighbor_row(
      query, nn_result, config_.reverse_nearest_neighbor_num);

  shared_ptr<column_table> table = mixable_scores_->get_model();
  for (size_t i = 0; i < nn_result.size(); ++i) {
    const std::pair<bool, uint64_t> hit =
        table->exact_match(nn_result[i].first);
    if (hit.first) {
      neighbors.insert(hit.second);
    }
  }
}

void light_lof::update_entries(const unordered_set<uint64_t>& neighbors) {
  shared_ptr<column_table> table = mixable_scores_->get_model();
  table::float_column& kdist_column =
      table->get_float_column(KDIST_COLUMN_INDEX);
  table::float_column& lrd_column = table->get_float_column(LRD_COLUMN_INDEX);

  unordered_map<uint64_t, std::vector<std::pair<uint64_t, float> > >
      nested_neighbors;

  // Gather k-nearest neighbors of each member of neighbors and update their
  // k-dists.
  std::vector<std::pair<std::string, float> > nn_result;
  for (unordered_set<uint64_t>::const_iterator it = neighbors.begin();
       it != neighbors.end(); ++it) {
    nearest_neighbor_engine_->neighbor_row(
        table->get_key(*it), nn_result, config_.nearest_neighbor_num);
    std::vector<std::pair<uint64_t, float> >& nn_indexes =
        nested_neighbors[*it];

    nn_indexes.reserve(nn_result.size());
    for (size_t i = 0; i < nn_result.size(); ++i) {
      const std::pair<bool, uint64_t> hit =
          table->exact_match(nn_result[i].first);
      if (hit.first) {
        nn_indexes.push_back(std::make_pair(hit.second, nn_result[i].second));
      }
    }

    kdist_column[*it] = nn_result.back().second;
  }

  // Calculate LRDs of neighbors.
  const table::owner owner(my_id_);
  for (unordered_set<uint64_t>::const_iterator it = neighbors.begin();
       it != neighbors.end(); ++it) {
    const std::vector<std::pair<uint64_t, float> >& nn = nested_neighbors[*it];
    float lrd = 1;
    if (!nn.empty()) {
      const size_t length = std::min(
          nn.size(), static_cast<size_t>(config_.nearest_neighbor_num));
      float sum_reachability = 0;
      for (size_t i = 0; i < length; ++i) {
        sum_reachability += std::max(nn[i].second, kdist_column[nn[i].first]);
      }

      if (sum_reachability == 0) {
        lrd = std::numeric_limits<float>::infinity();
      } else {
        lrd = length / sum_reachability;
      }
    }
    lrd_column[*it] = lrd;
    table->update_clock(*it, owner);
  }
}

light_lof::parameter light_lof::get_row_parameter(const std::string& row)
    const {
  shared_ptr<column_table> table = mixable_scores_->get_model();
  std::pair<bool, uint64_t> hit = table->exact_match(row);
  if (!hit.first) {
    throw JUBATUS_EXCEPTION(common::exception::runtime_error(
        "row \"" + row + "\" not found in light_lof table"));
  }
  parameter param;
  param.kdist = table->get_float_column(KDIST_COLUMN_INDEX)[hit.second];
  param.lrd = table->get_float_column(LRD_COLUMN_INDEX)[hit.second];
  return param;
}

}  // namespace anomaly
}  // namespace core
}  // namespace jubatus
