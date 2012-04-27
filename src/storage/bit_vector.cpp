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

#include <iostream>
#include "bit_vector.hpp"

using namespace std;

namespace jubatus {
namespace storage {

static const uint64_t BLOCKSIZE = 64;

bit_vector::bit_vector() : bit_num_(0) {}

bit_vector::~bit_vector() {}

bool bit_vector::operator==(const bit_vector& v) const {
  return v.bit_num() == bit_num() &&
      equal(bits_.begin(), bits_.end(), v.bits_.begin());
}

void bit_vector::resize_and_clear(uint64_t bit_num){
  bit_num_ = bit_num;
  bits_.resize((bit_num + BLOCKSIZE - 1) / BLOCKSIZE, 0);
}

void bit_vector::set_bit(uint64_t pos){
  bits_[pos / BLOCKSIZE] |= (1LLU << (pos % BLOCKSIZE));
}

uint64_t  bit_vector::calc_hamming_similarity(const bit_vector& bv) const{
  uint64_t match_num = 0;
  for (size_t i = 0; i < bits_.size() && i < bv.bits_.size(); ++i){
    uint64_t all_num = ((i+1) * BLOCKSIZE > bit_num_) ?  bit_num_ - i * BLOCKSIZE : BLOCKSIZE;
    match_num += all_num - pop_count(bits_[i] ^ bv.bits_[i]);
  }
  return match_num;
}
}
}
