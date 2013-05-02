// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "lof_storage.hpp"

#include <algorithm>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "anomaly_type.hpp"
#include "../common/exception.hpp"
#include "../common/jsonconfig.hpp"
#include "../recommender/euclid_lsh.hpp"
#include "../recommender/recommender_factory.hpp"

using jubatus::core::storage::lof_storage;
using pfi::data::serialization::binary_iarchive;
using pfi::data::serialization::binary_oarchive;
using pfi::data::unordered_map;
using pfi::data::unordered_set;
using std::istream;
using std::istringstream;
using std::max;
using std::min;
using std::numeric_limits;
using std::ostream;
using std::ostringstream;
using std::pair;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {

const uint32_t lof_storage::DEFAULT_NEIGHBOR_NUM = 10;
const uint32_t lof_storage::DEFAULT_REVERSE_NN_NUM = 30;

lof_storage::config::config()
    : nearest_neighbor_num(DEFAULT_NEIGHBOR_NUM),
      reverse_nearest_neighbor_num(DEFAULT_REVERSE_NN_NUM) {
}

lof_storage::lof_storage()
    : neighbor_num_(DEFAULT_NEIGHBOR_NUM),
      reverse_nn_num_(DEFAULT_REVERSE_NN_NUM),
      nn_engine_(recommender::recommender_factory::create_recommender(
          "euclid_lsh",
          jsonconfig::config(pfi::text::json::to_json(
              recommender::euclid_lsh::config())))) {
}

lof_storage::lof_storage(recommender::recommender_base* nn_engine)
    : neighbor_num_(DEFAULT_NEIGHBOR_NUM),
      reverse_nn_num_(DEFAULT_REVERSE_NN_NUM),
      nn_engine_(nn_engine) {
}

lof_storage::lof_storage(
    const config& config,
    recommender::recommender_base* nn_engine)
    : neighbor_num_(config.nearest_neighbor_num),
      reverse_nn_num_(config.reverse_nearest_neighbor_num),
      nn_engine_(nn_engine) {
}

lof_storage::~lof_storage() {
}

float lof_storage::collect_lrds(
    const sfv_t& query,
    unordered_map<string, float>& neighbor_lrd) const {
  vector<pair<string, float> > neighbors;
  nn_engine_->neighbor_row(query, neighbors, neighbor_num_);

  return collect_lrds_from_neighbors(neighbors, neighbor_lrd);
}

float lof_storage::collect_lrds(
    const string& id,
    unordered_map<string, float>& neighbor_lrd) const {
  vector<pair<string, float> > neighbors;
  nn_engine_->neighbor_row(id, neighbors, neighbor_num_ + 1);

  // neighbor_row returns id itself, so we remove it from the list
  for (size_t i = 0; i < neighbors.size(); ++i) {
    if (neighbors[i].first == id) {
      swap(neighbors[i], neighbors.back());
      neighbors.pop_back();
      break;
    }
  }

  return collect_lrds_from_neighbors(neighbors, neighbor_lrd);
}

void lof_storage::remove_row(const string& row) {
  mark_removed(lof_table_diff_[row]);
  nn_engine_->clear_row(row);
}

void lof_storage::clear() {
  lof_table_t().swap(lof_table_);
  lof_table_t().swap(lof_table_diff_);
  nn_engine_->clear();
}

void lof_storage::get_all_row_ids(vector<string>& ids) const {
  nn_engine_->get_all_row_ids(ids);
}

void lof_storage::update_row(const string& row, const sfv_t& diff) {
  unordered_set<string> update_set;

  {
    sfv_t query;
    nn_engine_->decode_row(row, query);
    if (!query.empty()) {
      collect_neighbors(row, update_set);
    }
  }

  nn_engine_->update_row(row, diff);
  collect_neighbors(row, update_set);

  update_set.insert(row);

  update_entries(update_set);
}

string lof_storage::name() const {
  return "lof_storage";
}

float lof_storage::get_kdist(const string& row) const {
  lof_table_t::const_iterator it = lof_table_diff_.find(row);
  if (it == lof_table_diff_.end()) {
    it = lof_table_.find(row);
    if (it == lof_table_.end()) {
      throw JUBATUS_EXCEPTION(
        exception::runtime_error("specified row does not exist")
        << exception::error_message("row id: " + row));
    }
  } else if (is_removed(it->second)) {
    throw JUBATUS_EXCEPTION(
      exception::runtime_error("specified row is recently removed")
      << exception::error_message("row id: " + row));
  }
  return it->second.kdist;
}

float lof_storage::get_lrd(const string& row) const {
  lof_table_t::const_iterator it = lof_table_diff_.find(row);
  if (it == lof_table_diff_.end()) {
    it = lof_table_.find(row);
    if (it == lof_table_.end()) {
      throw JUBATUS_EXCEPTION(
        exception::runtime_error("specified row does not exist")
        << exception::error_message("row id: " + row));
    }
  } else if (is_removed(it->second)) {
    throw JUBATUS_EXCEPTION(
      exception::runtime_error("specified row is recently removed")
      << exception::error_message("row id: " + row));
  }
  return it->second.lrd;
}

void lof_storage::update_all() {
  vector<string> ids;
  get_all_row_ids(ids);

  // NOTE: These two loops are separated, since update_lrd requires new kdist
  // values of k-NN.
  for (size_t i = 0; i < ids.size(); ++i) {
    update_kdist(ids[i]);
  }
  for (size_t i = 0; i < ids.size(); ++i) {
    update_lrd(ids[i]);
  }
}

bool lof_storage::save(ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << *this;
  return true;
}

bool lof_storage::load(istream& is) {
  binary_iarchive ia(is);
  ia >> *this;
  return true;
}

void lof_storage::set_nn_engine(recommender::recommender_base* nn_engine) {
  nn_engine_.reset(nn_engine);
}

void lof_storage::get_diff(string& diff) const {
  string nn_diff;
  nn_engine_->get_const_storage()->get_diff(nn_diff);

  ostringstream oss;
  serialize_diff(lof_table_diff_, nn_diff, oss);

  diff = oss.str();
}

void lof_storage::set_mixed_and_clear_diff(const string& mixed_diff) {
  string nn_diff;
  deserialize_diff(mixed_diff, lof_table_diff_, nn_diff);

  nn_engine_->get_storage()->set_mixed_and_clear_diff(nn_diff);

  for (lof_table_t::const_iterator it = lof_table_diff_.begin();
       it != lof_table_diff_.end(); ++it) {
    if (is_removed(it->second)) {
      lof_table_.erase(it->first);
    } else {
      lof_table_[it->first] = it->second;
    }
  }
  lof_table_diff_.clear();
}

void lof_storage::mix(const string& lhs, string& rhs) const {
  lof_table_t diff, mixed;
  string nn_diff, nn_mixed;

  deserialize_diff(lhs, diff, nn_diff);
  deserialize_diff(rhs, mixed, nn_mixed);

  nn_engine_->get_const_storage()->mix(nn_diff, nn_mixed);

  for (lof_table_t::const_iterator it = diff.begin(); it != diff.end(); ++it) {
    if (is_removed(it->second)) {
      mark_removed(mixed[it->first]);
    } else {
      mixed.insert(*it);
    }
  }

  ostringstream oss;
  serialize_diff(mixed, nn_mixed, oss);

  rhs = oss.str();
}

// private

// static
void lof_storage::mark_removed(lof_entry& entry) {
  entry.kdist = -1;
}

// static
bool lof_storage::is_removed(const lof_entry& entry) {
  return entry.kdist < 0;
}

float lof_storage::collect_lrds_from_neighbors(
    const vector<pair<string, float> >& neighbors,
    unordered_map<string, float>& neighbor_lrd) const {
  if (neighbors.empty()) {
    return numeric_limits<float>::infinity();
  }

  // collect lrd values of the nearest neighbors
  neighbor_lrd.clear();
  for (size_t i = 0; i < neighbors.size(); ++i) {
    neighbor_lrd[neighbors[i].first] = get_lrd(neighbors[i].first);
  }

  // return lrd of the query
  float sum_reachability = 0;
  for (size_t i = 0; i < neighbors.size(); ++i) {
    sum_reachability += max(neighbors[i].second, get_kdist(neighbors[i].first));
  }

  if (sum_reachability == 0) {
    return numeric_limits<float>::infinity();
  }

  return neighbors.size() / sum_reachability;
}

void lof_storage::serialize_diff(
    const lof_table_t& table,
    const string& nn_diff,
    ostream& out) const {
  binary_oarchive bo(out);
  string name = nn_engine_->type();
  bo << const_cast<lof_table_t&>(table) << name << const_cast<string&>(nn_diff);
}

void lof_storage::deserialize_diff(
    const string& diff,
    lof_table_t& table,
    string& nn_diff) const {
  string nn_engine_name;

  istringstream iss(diff);
  binary_iarchive bi(iss);
  bi >> table >> nn_engine_name;

  if (nn_engine_->type() != nn_engine_name) {
    throw JUBATUS_EXCEPTION(
      exception::runtime_error("inconsistent nearest neighbor engine type")
      << exception::error_message(
        "lof's NN engine type:  " + nn_engine_->type())
      << exception::error_message("diff's NN engine type: " + nn_engine_name));
  }

  bi >> nn_diff;
}

void lof_storage::collect_neighbors(
    const string& row,
    unordered_set<string>& nn) const {
  vector<pair<string, float> > neighbors;
  nn_engine_->neighbor_row(row, neighbors, reverse_nn_num_);

  for (size_t i = 0; i < neighbors.size(); ++i) {
    nn.insert(neighbors[i].first);
  }
}

void lof_storage::update_entries(const unordered_set<string>& rows) {
  // NOTE: These two loops are separated, since update_lrd requires new kdist
  // values of k-NN.
  typedef unordered_map<string, vector<pair<string, float> > >
    rows_to_neighbors_type;

  rows_to_neighbors_type rows_to_neighbors;
  for (unordered_set<string>::const_iterator it = rows.begin();
       it != rows.end(); ++it) {
    nn_engine_->neighbor_row(*it, rows_to_neighbors[*it], neighbor_num_);
  }

  for (rows_to_neighbors_type::const_iterator it = rows_to_neighbors.begin();
       it != rows_to_neighbors.end(); ++it) {
    update_kdist_with_neighbors(it->first, it->second);
  }
  for (rows_to_neighbors_type::const_iterator it = rows_to_neighbors.begin();
       it != rows_to_neighbors.end(); ++it) {
    update_lrd_with_neighbors(it->first, it->second);
  }
}

void lof_storage::update_kdist(const string& row) {
  vector<pair<string, float> > neighbors;
  nn_engine_->neighbor_row(row, neighbors, neighbor_num_);
  update_kdist_with_neighbors(row, neighbors);
}

void lof_storage::update_kdist_with_neighbors(
    const string& row,
    const vector<pair<string, float> >& neighbors) {
  if (!neighbors.empty()) {
    lof_table_diff_[row].kdist = neighbors.back().second;
  }
}

void lof_storage::update_lrd(const string& row) {
  vector<pair<string, float> > neighbors;
  nn_engine_->neighbor_row(row, neighbors, neighbor_num_);
  update_lrd_with_neighbors(row, neighbors);
}

void lof_storage::update_lrd_with_neighbors(
    const string& row, const vector<pair<string, float> >& neighbors) {
  if (neighbors.empty()) {
    lof_table_diff_[row].lrd = 1;
    return;
  }

  const size_t length = min(neighbors.size(), size_t(neighbor_num_));
  float sum_reachability = 0;
  for (size_t i = 0; i < length; ++i) {
    sum_reachability += max(neighbors[i].second, get_kdist(neighbors[i].first));
  }

  if (sum_reachability == 0) {
    lof_table_diff_[row].lrd = numeric_limits<float>::infinity();
    return;
  }

  lof_table_diff_[row].lrd = length / sum_reachability;
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
