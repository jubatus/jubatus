// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "minhash.hpp"

#include <algorithm>
#include <cmath>
#include <cfloat>
#include <string>
#include <utility>
#include <vector>
#include "../common/exception.hpp"
#include "../common/hash.hpp"

using std::pair;
using std::string;
using std::vector;
using jubatus::core::storage::bit_vector;

namespace jubatus {
namespace core {
namespace recommender {

const uint64_t minhash::hash_prime = 0xc3a5c85c97cb3127ULL;

minhash::minhash()
    : hash_num_(64) {
}

minhash::minhash(const config& config)
    : hash_num_(config.hash_num) {
}

minhash::~minhash() {
}

void minhash::similar_row(
    const sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  ids.clear();
  if (ret_num == 0) {
    return;
  }

  bit_vector query_bv;
  calc_minhash_values(query, query_bv);
  row2minhashvals_.similar_row(query_bv, ids, ret_num);
}

void minhash::neighbor_row(
    const sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  similar_row(query, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = 1 - ids[i].second;
  }
}

void minhash::clear() {
  orig_.clear();
  row2minhashvals_.clear();
}

void minhash::clear_row(const string& id) {
  orig_.remove_row(id);
  row2minhashvals_.remove_row(id);
}

void minhash::calc_minhash_values(const sfv_t& sfv, bit_vector& bv) const {
  vector<float> min_values_buffer(hash_num_, FLT_MAX);
  vector<uint64_t> hash_buffer(hash_num_);
  for (size_t i = 0; i < sfv.size(); ++i) {
    uint64_t key_hash = hash_util::calc_string_hash(sfv[i].first);
    float val = sfv[i].second;
    for (uint64_t j = 0; j < hash_num_; ++j) {
      float hashval = calc_hash(key_hash, j, val);
      if (hashval < min_values_buffer[j]) {
        min_values_buffer[j] = hashval;
        hash_buffer[j] = key_hash;
      }
    }
  }

  bv.resize_and_clear(hash_num_);
  for (size_t i = 0; i < hash_buffer.size(); ++i) {
    if ((hash_buffer[i] & 1LLU) == 1) {
      bv.set_bit(i);
    }
  }
}

void minhash::update_row(const string& id, const sfv_diff_t& diff) {
  orig_.set_row(id, diff);

  sfv_t row;
  orig_.get_row(id, row);
  bit_vector bv;
  calc_minhash_values(row, bv);
  row2minhashvals_.set_row(id, bv);
}

void minhash::get_all_row_ids(std::vector<std::string>& ids) const {
  row2minhashvals_.get_all_row_ids(ids);
}

// original by Hash64 http://burtleburtle.net/bob/hash/evahash.html
void minhash::hash_mix64(uint64_t& a, uint64_t& b, uint64_t& c) {
  a -= b;
  a -= c;
  a ^= (c >> 43);
  b -= c;
  b -= a;
  b ^= (a << 9);
  c -= a;
  c -= b;
  c ^= (b >> 8);
  a -= b;
  a -= c;
  a ^= (c >> 38);
  b -= c;
  b -= a;
  b ^= (a << 23);
  c -= a;
  c -= b;
  c ^= (b >> 5);
  a -= b;
  a -= c;
  a ^= (c >> 35);
  b -= c;
  b -= a;
  b ^= (a << 49);
  c -= a;
  c -= b;
  c ^= (b >> 11);
  a -= b;
  a -= c;
  a ^= (c >> 12);
  b -= c;
  b -= a;
  b ^= (a << 18);
  c -= a;
  c -= b;
  c ^= (b >> 22);
}

float minhash::calc_hash(uint64_t a, uint64_t b, float val) {
  uint64_t c = hash_prime;
  hash_mix64(a, b, c);
  hash_mix64(a, b, c);
  float r = static_cast<float>(a) / static_cast<float>(0xFFFFFFFFFFFFFFFFLLU);
  return -log(r) / val;
}

string minhash::type() const {
  return string("minhash");
}
bool minhash::save_impl(std::ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << row2minhashvals_;
  return true;
}
bool minhash::load_impl(std::istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> row2minhashvals_;
  return true;
}
core::storage::recommender_storage_base* minhash::get_storage() {
  return &row2minhashvals_;
}
const core::storage::recommender_storage_base*
    minhash::get_const_storage() const {
  return &row2minhashvals_;
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
