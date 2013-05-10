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

#include "euclid_lsh.hpp"

#include <cmath>
#include <queue>
#include <utility>
#include <string>
#include <vector>
#include <pficommon/data/serialization.h>
#include <pficommon/lang/cast.h>
#include <pficommon/math/random.h>
#include "../common/hash.hpp"
#include "../storage/lsh_util.hpp"
#include "../storage/lsh_vector.hpp"

using std::string;
using std::vector;
using std::pair;
using std::ostream;
using std::istream;
using pfi::math::random::mtrand;

namespace jubatus {
namespace core {
namespace recommender {

namespace {

struct greater_second {
  bool operator()(
      const pair<string, float>& l,
      const pair<string, float>& r) const {
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

vector<float> lsh_function(
    const sfv_t& query,
    size_t dimension,
    float bin_width) {
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

}  // namespace

euclid_lsh::config::config()
    : lsh_num(DEFAULT_LSH_NUM),
      table_num(DEFAULT_TABLE_NUM),
      bin_width(DEFAULT_BIN_WIDTH),
      probe_num(DEFAULT_NUM_PROBE),
      seed(DEFAULT_SEED),
      retain_projection(DEFAULT_RETAIN_PROJECTION) {
}

const uint64_t euclid_lsh::DEFAULT_LSH_NUM = 64;  // should be in config
const uint64_t euclid_lsh::DEFAULT_TABLE_NUM = 4;  // should be in config
const float euclid_lsh::DEFAULT_BIN_WIDTH = 100;  // should be in config
const uint32_t euclid_lsh::DEFAULT_NUM_PROBE = 64;  // should be in config
const uint32_t euclid_lsh::DEFAULT_SEED = 1091;  // should be in config
const bool euclid_lsh::DEFAULT_RETAIN_PROJECTION = false;

euclid_lsh::euclid_lsh()
    : lsh_index_(DEFAULT_LSH_NUM, DEFAULT_TABLE_NUM, DEFAULT_SEED),
      bin_width_(DEFAULT_BIN_WIDTH),
      num_probe_(DEFAULT_NUM_PROBE),
      retain_projection_(DEFAULT_RETAIN_PROJECTION) {
}

euclid_lsh::euclid_lsh(const config& config)
    : lsh_index_(config.lsh_num, config.table_num, config.seed),
      bin_width_(config.bin_width),
      num_probe_(config.probe_num),
      retain_projection_(config.retain_projection) {
}

euclid_lsh::~euclid_lsh() {
}

void euclid_lsh::neighbor_row(
    const sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  similar_row(query, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = -ids[i].second;
  }
}

void euclid_lsh::neighbor_row(
    const string& id,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  similar_row(id, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = -ids[i].second;
  }
}

void euclid_lsh::similar_row(
    const sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  ids.clear();

  const vector<float> hash = lsh_function(query,
                                          lsh_index_.all_lsh_num(),
                                          bin_width_);
  const float norm = calc_norm(query);
  lsh_index_.similar_row(hash, norm, num_probe_, ret_num, ids);
}

void euclid_lsh::similar_row(
    const string& id,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  ids.clear();
  lsh_index_.similar_row(id, ret_num, ids);
}

void euclid_lsh::clear() {
  orig_.clear();
  lsh_index_.clear();

  // Clear projection cache
  pfi::data::unordered_map<uint32_t, std::vector<float> >().swap(projection_);
}

void euclid_lsh::clear_row(const string& id) {
  orig_.remove_row(id);
  lsh_index_.remove_row(id);
}

void euclid_lsh::update_row(const string& id, const sfv_diff_t& diff) {
  orig_.set_row(id, diff);
  sfv_t row;
  orig_.get_row(id, row);

  const vector<float> hash = lsh_function(row,
                                          lsh_index_.all_lsh_num(),
                                          bin_width_);
  const float norm = calc_norm(row);
  lsh_index_.set_row(id, hash, norm);
}

void euclid_lsh::get_all_row_ids(vector<string>& ids) const {
  lsh_index_.get_all_row_ids(ids);
}

string euclid_lsh::type() const {
  return "euclid_lsh";
}

core::storage::lsh_index_storage* euclid_lsh::get_storage() {
  return &lsh_index_;
}

const core::storage::lsh_index_storage* euclid_lsh::get_const_storage() const {
  return &lsh_index_;
}

vector<float> euclid_lsh::calculate_lsh(const sfv_t& query) {
  vector<float> hash(lsh_index_.all_lsh_num());
  for (size_t i = 0; i < query.size(); ++i) {
    const uint32_t seed = hash_util::calc_string_hash(query[i].first);
    const vector<float> proj = get_projection(seed);
    for (size_t j = 0; j < hash.size(); ++j) {
      hash[j] += query[i].second * proj[j];
    }
  }
  for (size_t j = 0; j < hash.size(); ++j) {
    hash[j] /= bin_width_;
  }
  return hash;
}

vector<float> euclid_lsh::get_projection(uint32_t seed) {
  vector<float> tmpl_proj;
  vector<float>& proj = retain_projection_ ? projection_[seed] : tmpl_proj;

  if (proj.empty()) {
    mtrand rnd(seed);
    proj.resize(lsh_index_.all_lsh_num());
    for (size_t i = 0; i < proj.size(); ++i) {
      proj[i] = rnd.next_gaussian();
    }
  }
  return proj;
}

bool euclid_lsh::save_impl(ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << *this;
  return true;
}

bool euclid_lsh::load_impl(istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> *this;
  return true;
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
