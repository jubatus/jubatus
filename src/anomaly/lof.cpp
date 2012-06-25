// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <cmath>
#include <string>
#include <vector>
#include <glog/logging.h>
#include <pficommon/data/serialization.h>
#include <pficommon/lang/cast.h>
#include <pficommon/math/random.h>
#include "lof.hpp"
#include "../common/hash.hpp"
#include "../common/config_util.hpp"
#include "../storage/lsh_util.hpp"
#include "../storage/lsh_vector.hpp"

using namespace std;
using namespace jubatus::storage;
using pfi::data::unordered_map;
using pfi::math::random::mtrand;

namespace jubatus {
namespace anomaly {

namespace {

float calculate_lof(float lrd,
                    const unordered_map<string, float>& neighbor_lrd) {
  float sum_neighbor_lrd = 0;
  for (unordered_map<string, float>::const_iterator it = neighbor_lrd.begin();
       it != neighbor_lrd.end(); ++it) {
    sum_neighbor_lrd += it->second;
  }

  return sum_neighbor_lrd / (neighbor_lrd.size() * lrd);
}

}

lof::lof() {
}

lof::lof(const map<string, string>& config)
    : lof_index_(config) {
}

lof::~lof() {
}

float lof::calc_anomaly_score(const sfv_t& query) const {
  unordered_map<string, float> neighbor_lrd;
  const float lrd = lof_index_.collect_lrds(query, neighbor_lrd);

  return calculate_lof(lrd, neighbor_lrd);
}

float lof::calc_anomaly_score(const string& id) const {
  unordered_map<string, float> neighbor_lrd;
  const float lrd = lof_index_.collect_lrds(id, neighbor_lrd);

  return calculate_lof(lrd, neighbor_lrd);
}

void lof::clear() {
  lof_index_.clear();
}

void lof::clear_row(const string& id) {
  lof_index_.remove_row(id);
}

void lof::update_row(string& id, const sfv_diff_t& diff) {
  lof_index_.update_row(id, diff);
}

void lof::get_all_row_ids(vector<string>& ids) const {
  lof_index_.get_all_row_ids(ids);
}

string lof::type() const {
  return "lof";
}

storage::anomaly_storage_base* lof::get_storage() {
  return &lof_index_;
}

const storage::anomaly_storage_base* lof::get_const_storage() const {
  return &lof_index_;
}

bool lof::save_impl(ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << lof_index_;
  return true;
}

bool lof::load_impl(istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> lof_index_;
  return true;
}


}
}
