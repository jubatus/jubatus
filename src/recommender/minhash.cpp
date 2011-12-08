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

#include <algorithm>
#include <cmath>
#include "minhash.hpp"
#include "../common/exception.hpp"

using namespace std;
using namespace pfi::data;
using namespace pfi::lang;

namespace jubatus {
namespace recommender {

static const uint64_t DEFAULT_BASE_NUM = 32; // should be in config
static const uint64_t DEFAULT_SEARCH_NUM = 16; // shoud be config

const uint64_t minhash::hash_prime = 0xc3a5c85c97cb3127ULL;

minhash::minhash(shared_ptr<storage::recommender_storage> storage) : 
  recommender_base(storage) {
}

minhash::~minhash(){
}

void minhash::similar_row(const sfv_t& query, vector<pair<string, float> > & ids, size_t ret_num) const{
  throw unsupported_method("minhash::similar_row()"); // not supported yet.
}

void minhash::clear(){
  origs_->clear();
  
}

void minhash::clear_row(const string& id){
  throw unsupported_method("minhash::clear_row()"); // not supported yet.
}

void minhash::calc_minhash_values(const sfv_t& sfv, vector<float>& values) const{
  
}

void minhash::update_row(const string& id, const sfv_diff_t& diff){
  origs_->set_row(id, diff);
}

void minhash::hash_mix64(uint64_t& a, uint64_t& b, uint64_t& c) {
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

float minhash::calc_hash(uint64_t a, uint64_t b, float val){
  uint64_t c = hash_prime;
  hash_mix64(a, b, c);
  hash_mix64(a, b, c);
  float r = static_cast<float>(a) / static_cast<float>(0xFFFFFFFFFFFFFFFFLLU);
  return - log(r) / val;
}



} // namespace recommender
} // namespace jubatus

