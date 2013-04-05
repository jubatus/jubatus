// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <map>
#include <string>
#include <vector>

#include "minhash.hpp"

#include <cfloat>
#include <cmath>
#include "../common/config_util.hpp"
#include "../common/hash.hpp"

using std::string;
using std::map;
using std::vector;
using std::pair;
using pfi::lang::lexical_cast;
using jubatus::table::column_table;
using jubatus::table::bit_vector;
using jubatus::table::column_type;


namespace jubatus {
namespace nearest_neighbor {

namespace {
void hash_mix64(uint64_t& a, uint64_t& b, uint64_t& c) {
  a -= b; a -= c; a ^= (c>>43);
  b -= c; b -= a; b ^= (a<<9);
  c -= a; c -= b; c ^= (b>>8);
  a -= b; a -= c; a ^= (c>>38);
  b -= c; b -= a; b ^= (a<<23);
  c -= a; c -= b; c ^= (b>>5);
  a -= b; a -= c; a ^= (c>>35);
  b -= c; b -= a; b ^= (a<<49);
  c -= a; c -= b; c ^= (b>>11);
  a -= b; a -= c; a ^= (c>>12);
  b -= c; b -= a; b ^= (a<<18);
  c -= a; c -= b; c ^= (b>>22);
}

float calc_hash(uint64_t a, uint64_t b, float val) {
  static const uint64_t HASH_PRIME = 0xc3a5c85c97cb3127ULL;

  uint64_t c = HASH_PRIME;
  hash_mix64(a, b, c);
  hash_mix64(a, b, c);
  float r = static_cast<float>(a) / static_cast<float>(0xFFFFFFFFFFFFFFFFLLU);
  return - log(r) / val;
}
}

minhash::minhash(const map<string, string>& config,
                 table::column_table* table, const std::string& id)
    : bit_vector_nearest_neighbor_base(
      get_param(config, "minhash:bitnum", 64u), table, id) {}

minhash::minhash(const map<string, string>& config,
                 table::column_table* table,
                 vector<column_type>& schema,
                 const std::string& id)
    : bit_vector_nearest_neighbor_base(
      get_param(config, "minhash:bitnum", 64u), table, schema, id) {}

bit_vector minhash::hash(const sfv_t& sfv) const {
  vector<float> min_values_buffer(bitnum(), FLT_MAX);
  vector<uint64_t> hash_buffer(bitnum());
  for (size_t i = 0; i < sfv.size(); ++i) {
    uint64_t key_hash = hash_util::calc_string_hash(sfv[i].first);
    float val = sfv[i].second;
    for (uint32_t j = 0; j < bitnum(); ++j) {
      float hashval = calc_hash(key_hash, j, val);
      if (hashval < min_values_buffer[j]) {
        min_values_buffer[j] = hashval;
        hash_buffer[j] = key_hash;
      }
    }
  }

  bit_vector bv(bitnum());
  for (size_t i = 0; i < hash_buffer.size(); ++i) {
    if ((hash_buffer[i] & 1LLU) == 1) {
      bv.set_bit(i);
    }
  }

  return bv;
}

}  // namespace nearest_neighbor
}  // namespace jubatus
