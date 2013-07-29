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

#include "lof.hpp"

#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include <pficommon/data/serialization.h>
#include <pficommon/lang/cast.h>
#include <pficommon/math/random.h>

#include "../common/hash.hpp"
#include "../storage/lsh_util.hpp"
#include "../storage/lsh_vector.hpp"

using jubatus::core::anomaly::lof;
using pfi::data::unordered_map;
using pfi::math::random::mtrand;
using std::isinf;
using std::istream;
using std::ostream;
using std::numeric_limits;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace anomaly {

namespace {

float calculate_lof(
    float lrd,
    const unordered_map<string, float>& neighbor_lrd) {
  if (neighbor_lrd.empty()) {
    return lrd == 0 ? 1 : numeric_limits<float>::infinity();
  }

  float sum_neighbor_lrd = 0;
  for (unordered_map<string, float>::const_iterator it = neighbor_lrd.begin();
       it != neighbor_lrd.end(); ++it) {
    sum_neighbor_lrd += it->second;
  }

  if (isinf(sum_neighbor_lrd) && isinf(lrd)) {
    return 1;
  }

  return sum_neighbor_lrd / (neighbor_lrd.size() * lrd);
}

}  // namespace

lof::lof() {
}

lof::lof(
    const storage::lof_storage::config& config,
    recommender::recommender_base* nn_engine)
    : lof_index_(new storage::lof_storage(config, nn_engine)) {
  mixable_storage_.set_model(lof_index_);
}

lof::~lof() {
}

float lof::calc_anomaly_score(const common::sfv_t& query) const {
  unordered_map<string, float> neighbor_lrd;
  const float lrd = lof_index_->collect_lrds(query, neighbor_lrd);

  return calculate_lof(lrd, neighbor_lrd);
}

float lof::calc_anomaly_score(const string& id) const {
  unordered_map<string, float> neighbor_lrd;
  const float lrd = lof_index_->collect_lrds(id, neighbor_lrd);

  return calculate_lof(lrd, neighbor_lrd);
}

void lof::clear() {
  lof_index_->clear();
}

void lof::clear_row(const string& id) {
  lof_index_->remove_row(id);
}

void lof::update_row(const string& id, const sfv_diff_t& diff) {
  lof_index_->update_row(id, diff);
}

void lof::set_row(const string& id, const common::sfv_t& sfv) {
  throw JUBATUS_EXCEPTION(common::unsupported_method(__func__));
}

void lof::get_all_row_ids(vector<string>& ids) const {
  lof_index_->get_all_row_ids(ids);
}

string lof::type() const {
  return "lof";
}

void lof::register_mixables_to_holder(
    pfi::lang::shared_ptr<framework::mixable_holder> holder) {
  holder->register_mixable(&mixable_storage_);
}

}  // namespace anomaly
}  // namespace core
}  // namespace jubatus
