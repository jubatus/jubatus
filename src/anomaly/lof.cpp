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
using pfi::math::random::mtrand;

namespace jubatus {
namespace anomaly {

namespace {

/*
const uint32_t DEFAULT_LSH_NUM = 64;  // should be in config
const uint32_t DEFAULT_TABLE_NUM = 4;  // should be in config
const float DEFAULT_BIN_WIDTH = 100;  // should be in config
const uint32_t DEFAULT_NUM_PROBE = 64;  // should be in config
const uint32_t DEFAULT_SEED = 1091;  // should be in config

const map<string, string> DEFAULT_NN_CONFIG = {
  {"nn_method", "euclid_lsh"},
  {"lsh_num", "64"},
  {"table_num", "4"},
  {"bin_width", "100"},
  {"num_probe", "64"},
  {"seed", "1091"}
};
*/

const uint32_t DEFAULT_NEIGHBOR_NUM = 10;  // should be in config
const float DEFAULT_REVERSE_NN_COEFFICIENT = 3;  // should be in config

struct greater_second {
  bool operator()(const pair<string, float>& l, const pair<string, float>& r) const {
    return l.second > r.second;
  }
};

float calc_norm(const sfv_t& sfv) {
  float sqnorm = 0;
  for (size_t i = 0; i < sfv.size(); ++i) {
    sqnorm += sfv[i].second * sfv[i].second;
  }
  return sqrt(sqnorm);
}

}

lof::lof()
  : lof_index_(),
    neighbor_num_(DEFAULT_NEIGHBOR_NUM),
    reverse_nn_coefficient_(DEFAULT_REVERSE_NN_COEFFICIENT) {
}

lof::lof(const map<string, string>& config)
  : lof_index_(config),
    neighbor_num_(get_param(config, "neighbor_num", DEFAULT_NEIGHBOR_NUM)),
  reverse_nn_coefficient_(get_param(config, "reverse_nn_coefficient", DEFAULT_REVERSE_NN_COEFFICIENT)) {
  const uint32_t neighbor_num = get_param(config, "neighbor_num", DEFAULT_NEIGHBOR_NUM);
  const float reverse_nn_coefficient = get_param(config, "reverse_nn_coefficient", DEFAULT_REVERSE_NN_COEFFICIENT);
  LOG(INFO) << "neighbor_num:" << neighbor_num;
  LOG(INFO) << "reverse_nn_coefficient:" << reverse_nn_coefficient;
}

lof::~lof() {
}

float lof::calc_anomaly_score(const string& id, const size_t neighbor_num) {
  pair<string, float> lrd_value_self;
  lrd_value_self.first = id;
  lrd_value_self.second = lof_index_.get_lrd(id);

  return lof::calc_anomaly_score(lrd_value_self.first, lrd_value_self.second, neighbor_num);
}

float lof::calc_anomaly_score(const sfv_t& query, const size_t neighbor_num) {
  pair<string, float> lrd_value_self;
  lof_index_.calc_lrd(query, lrd_value_self, neighbor_num);

  return lof::calc_anomaly_score(lrd_value_self.first, lrd_value_self.second, neighbor_num);
}

float lof::calc_anomaly_score(const string& row,
                              const float lrd,
                              const size_t neighbor_num) {
  map<string, float> neighbor_lrd_values;
  lof_index_.similar_row_lrds(row, neighbor_lrd_values, neighbor_num);
  
  float average_neighbor_lrd = 0;
  for (map<string, float>::const_iterator it = neighbor_lrd_values.begin(); it != neighbor_lrd_values.end(); ++it) {
    average_neighbor_lrd += it->second / neighbor_num;
  }

  return average_neighbor_lrd / lrd;
}

void lof::clear() {
  lof_index_.clear();
}

void lof::clear_row(const string& id) {
  lof_index_.remove_row(id);
}

  void lof::update_row(string& id, const sfv_diff_t& diff) {
  // hash value should be computed in lof_index_
  lof_index_.update_row(id, diff);

  vector<pair<string, float> > distance_values;
  lof_index_.similar_row(id, distance_values, (size_t) neighbor_num_ * reverse_nn_coefficient_);
  
  for(vector<pair<string, float> >::const_iterator it = distance_values.begin(); it != distance_values.end(); ++it) {
    lof_index_.set_kdist(it->first, max(lof_index_.get_kdist(it->first), it->second));
  }

  // TODO: Implement Update
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
