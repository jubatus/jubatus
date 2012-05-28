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
#include <queue>
#include <vector>
#include <glog/logging.h>
#include <pficommon/data/serialization.h>
#include <pficommon/lang/cast.h>
#include <pficommon/math/random.h>
#include "lof.hpp"
#include "../common/hash.hpp"
#include "../storage/lsh_util.hpp"
#include "../storage/lsh_vector.hpp"

using namespace std;
using namespace jubatus::storage;
using pfi::math::random::mtrand;

namespace jubatus {
namespace anomaly {

namespace {


const uint64_t DEFAULT_LSH_NUM = 64;  // should be in config
const uint64_t DEFAULT_TABLE_NUM = 4;  // should be in config
const float DEFAULT_BIN_WIDTH = 100;  // should be in config
const uint32_t DEFAULT_NUM_PROBE = 64;  // should be in config
const uint32_t DEFAULT_SEED = 1091;  // should be in config

const uint32_t DEFAULT_NEIGHBOR_NUM = 10;  // should be in config
const uint32_t DEFAULT_REVERSE_NN_COEFFICIENT = 3;  // should be in config

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

template<typename T>
T get_param(const map<string, string>& config, const string& name, T default_value) {
  const map<string, string>::const_iterator it = config.find(name);
  if (it == config.end()) {
    return default_value;
  }
  return pfi::lang::lexical_cast<T>(it->second);
}

}

lof::lof()
  : lof_index_(DEFAULT_NEIGHBOR_NUM, DEFAULT_REVERSE_NN_COEFFICIENT, DEFAULT_LSH_NUM, DEFAULT_TABLE_NUM, DEFAULT_SEED),
      bin_width_(DEFAULT_BIN_WIDTH),
      num_probe_(DEFAULT_NUM_PROBE) {
}

lof::lof(const std::map<std::string, std::string>& config)
  : lof_index_(get_param(config, "neighbor_num", DEFAULT_NEIGHBOR_NUM),
	       get_param(config. "reverse_nn_coefficient", DEFAULT_REVERSE_NN_COEFFICIENT),
	       get_param(config, "lsh_num", DEFAULT_LSH_NUM),
	       get_param(config, "table_num", DEFAULT_TABLE_NUM),
	       get_param(config, "seed", DEFAULT_SEED)),
    bin_width_(get_param(config, "bin_width", DEFAULT_BIN_WIDTH)),
    num_probe_(get_param(config, "probe_num", DEFAULT_NUM_PROBE)) {
  const uint64_t neighbor_num = get_param(config, "neighbor_num", DEFAULT_NEIGHBOR_NUM);
  const float reverse_nn_coefficient = get_param(config. "reverse_nn_coefficient", DEFAULT_REVERSE_NN_COEFFICIENT);
  LOG(INFO) << "neighbor_num:" << neighbor_num;
  LOF(INFO) << "reverse_nn_coefficient:" << reverse_nn_coefficient;
}

lof::~lof() {
}

void lof::calc_anomaly_score(const sfv_t& query, std::pair<std::string, float>& score, size_t neighbor_num) const {
  ids.clear();

  std::pair<std::string, float> lrd_self;
  lof_index_.calc_lrd(query, neibor_num. lrd_self);

  std::vector<std::string, float> lrds;
  lof_index_.similar_row_lrds(query, neighbor_num, lrds);
  float average_neighbor_lrd = 0;
  for (uint64_t i = 0; i < neighbor_num; ++i) {
    average_neibor_lrd += neighbors[i].second / neighbor_num;
  }

  ids.first = lrd_self.first;
  ids.second = average_neigbor_lrd / lrd_self.first;
}


void lof::clear() {
  orig_.clear();
  lof_index_.clear();
}

void lof::clear_row(const string& id) {
  orig_.remove_row(id);
  lof_index_.remove_row(id);
}

void lof::update_row(const string& id, const sfv_diff_t& diff) {
  orig_.set_row(id, diff);
  sfv_t row;
  orig_.get_row(id, row);

  //hash value should be computed in lof_index_
  // const vector<float> hash = lsh_function(row, lof_index_.all_lsh_num(), bin_width_);
  const float norm = calc_norm(row);
  //lof_index_.set_row(id, hash, norm);
  lof_index_.set_row(id, norm);
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
  pfi::data::serialization::binary_o archive oa(os);
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
