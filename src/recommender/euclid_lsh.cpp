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
#include "euclid_lsh.hpp"
#include "../common/config_util.hpp"
#include "../common/hash.hpp"
#include "../storage/lsh_util.hpp"
#include "../storage/lsh_vector.hpp"

using namespace std;
using namespace jubatus::storage;
using pfi::math::random::mtrand;

namespace jubatus {
namespace recommender {

namespace {

const uint64_t DEFAULT_LSH_NUM = 64;  // should be in config
const uint64_t DEFAULT_TABLE_NUM = 4;  // should be in config
const float DEFAULT_BIN_WIDTH = 100;  // should be in config
const uint32_t DEFAULT_NUM_PROBE = 64;  // should be in config
const uint32_t DEFAULT_SEED = 1091;  // should be in config

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

vector<float> lsh_function(const sfv_t& query, size_t dimension, float bin_width) {
  vector<float> hash(dimension);
  for (size_t i = 0; i < query.size(); ++i) {
    const uint32_t seed = hash_util::calc_string_hash(query[i].first);
    mtrand rnd(seed);
    for (size_t j = 0; j < hash.size(); ++j) {
      hash[j] += query[i].second * rnd.next_gaussian();
    }
  }
  for (size_t j = 0; j < dimension; ++j) {
    hash[j] /= bin_width;
  }
  return hash;
}

}

euclid_lsh::euclid_lsh()
    : lsh_index_(DEFAULT_LSH_NUM, DEFAULT_TABLE_NUM, DEFAULT_SEED),
      bin_width_(DEFAULT_BIN_WIDTH),
      num_probe_(DEFAULT_NUM_PROBE) {
}

euclid_lsh::euclid_lsh(const std::map<std::string, std::string>& config)
    : lsh_index_(get_param(config, "lsh_num", DEFAULT_LSH_NUM),
                 get_param(config, "table_num", DEFAULT_TABLE_NUM),
                 get_param(config, "seed", DEFAULT_SEED)),
      bin_width_(get_param(config, "bin_width", DEFAULT_BIN_WIDTH)),
      num_probe_(get_param(config, "probe_num", DEFAULT_NUM_PROBE)) {
}

euclid_lsh::~euclid_lsh() {
}

void euclid_lsh::neighbor_row(const sfv_t& query,
                             vector<pair<string, float> >& ids,
                             size_t ret_num) const {
  similar_row(query, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = -ids[i].second;
  }
}

void euclid_lsh::neighbor_row(const string& id,
                              vector<pair<string, float> >& ids,
                              size_t ret_num) const {
  similar_row(id, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = -ids[i].second;
  }
}

void euclid_lsh::similar_row(const sfv_t& query,
                             vector<pair<string, float> >& ids,
                             size_t ret_num) const {
  ids.clear();

  const vector<float> hash = lsh_function(query, lsh_index_.all_lsh_num(), bin_width_);
  const float norm = calc_norm(query);
  lsh_index_.similar_row(hash, norm, num_probe_, ret_num, ids);
}

void euclid_lsh::similar_row(const string& id,
                             vector<pair<string, float> >& ids,
                             size_t ret_num) const {
  ids.clear();
  lsh_index_.similar_row(id, ret_num, ids);
}

void euclid_lsh::clear() {
  orig_.clear();
  lsh_index_.clear();
}

void euclid_lsh::clear_row(const string& id) {
  orig_.remove_row(id);
  lsh_index_.remove_row(id);
}

void euclid_lsh::update_row(const string& id, const sfv_diff_t& diff) {
  orig_.set_row(id, diff);
  sfv_t row;
  orig_.get_row(id, row);

  const vector<float> hash = lsh_function(row, lsh_index_.all_lsh_num(), bin_width_);
  const float norm = calc_norm(row);
  lsh_index_.set_row(id, hash, norm);
}

void euclid_lsh::get_all_row_ids(vector<string>& ids) const {
  lsh_index_.get_all_row_ids(ids);
}

string euclid_lsh::type() const {
  return "euclid_lsh";
}

storage::lsh_index_storage* euclid_lsh::get_storage() {
  return &lsh_index_;
}

const storage::lsh_index_storage* euclid_lsh::get_const_storage() const {
  return &lsh_index_;
}

bool euclid_lsh::save_impl(ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << lsh_index_;
  return true;
}

bool euclid_lsh::load_impl(istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> lsh_index_;
  return true;
}

}
}
