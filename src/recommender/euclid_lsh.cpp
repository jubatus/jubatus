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
#include <pficommon/math/random.h>
#include "euclid_lsh.hpp"
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

vector<float> raw_lsh(const sfv_t& query, const vector<float>& shift, float bin_width) {
  vector<float> hash(shift.size());
  for (size_t i = 0; i < query.size(); ++i) {
    const uint32_t seed = hash_util::calc_string_hash(query[i].first);
    mtrand rnd(seed);
    for (size_t j = 0; j < hash.size(); ++j) {
      hash[j] += query[i].second * rnd.next_gaussian();
    }
  }

  for (size_t j = 0; j < hash.size(); ++j) {
    hash[j] = (hash[j] + shift[j]) / bin_width;
  }
  return hash;
}

lsh_vector lsh_function(const sfv_t& query,
                        const vector<float>& shift,
                        float bin_width) {
  vector<float> hash = raw_lsh(query, shift, bin_width);
  lsh_vector lv(hash.size());
  for (size_t i = 0; i < hash.size(); ++i) {
    lv.set(i, floor(hash[i]));
  }
  return lv;
}

}

euclid_lsh::euclid_lsh()
    : lsh_index_(DEFAULT_TABLE_NUM),
      bin_width_(DEFAULT_BIN_WIDTH),
      num_probe_(DEFAULT_NUM_PROBE) {
  initialize_shift(DEFAULT_LSH_NUM * DEFAULT_TABLE_NUM, DEFAULT_SEED);
}

euclid_lsh::euclid_lsh(uint64_t lsh_num,
                       uint64_t table_num,
                       float bin_width,
                       uint32_t num_probe,
                       uint32_t seed)
    : lsh_index_(table_num),
      bin_width_(bin_width),
      num_probe_(num_probe) {
  initialize_shift(lsh_num * table_num, seed);
}

euclid_lsh::~euclid_lsh() {
}

void euclid_lsh::similar_row(const sfv_t& query,
                             vector<pair<string, float> >& ids,
                             size_t ret_num) const {
  ids.clear();

  if (num_probe_ == 1) {
    const lsh_vector query_lv = lsh_function(query, shift_, bin_width_);
    lsh_index_.similar_row(query_lv, ids, ret_num);
  } else {
    const vector<float> query_hash = raw_lsh(query, shift_, bin_width_);
    lsh_index_.multi_probe_similar_row(query_hash, ids, num_probe_, ret_num);
  }
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

  const lsh_vector lv = lsh_function(row, shift_, bin_width_);
  lsh_index_.set_row(id, lv);
}

void euclid_lsh::get_all_row_ids(vector<string>& ids) const {
  lsh_index_.get_all_row_ids(ids);
}

string euclid_lsh::type() const {
  return "euclid_lsh";
}

storage::recommender_storage_base* euclid_lsh::get_storage() {
  return &lsh_index_;
}

const storage::recommender_storage_base* euclid_lsh::get_const_storage() const {
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

void euclid_lsh::initialize_shift(size_t lsh_num, uint32_t seed) {
  mtrand rnd(seed);
  shift_.reserve(lsh_num);
  for (size_t i = 0; i < lsh_num; ++i) {
    shift_.push_back(rnd.next_double(bin_width_));
  }
}

}
}
