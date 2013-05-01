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

#include "bit_vector.hpp"
#include <algorithm>
#include <iostream>

using std::equal;

namespace jubatus {
namespace core {
namespace storage {

static const uint64_t BLOCKSIZE = 64;

bit_vector::bit_vector()
    : bit_num_(0) {
}

bit_vector::~bit_vector() {
}

bool bit_vector::operator==(const bit_vector& v) const {
  return v.bit_num() == bit_num()
      && equal(bits_.begin(), bits_.end(), v.bits_.begin());
}

void bit_vector::resize_and_clear(uint64_t bit_num) {
  bit_num_ = bit_num;
  bits_.resize((bit_num + BLOCKSIZE - 1) / BLOCKSIZE, 0);
}

void bit_vector::set_bit(uint64_t pos) {
  bits_[pos / BLOCKSIZE] |= (1LLU << (pos % BLOCKSIZE));
}

uint64_t bit_vector::calc_hamming_similarity(const bit_vector& bv) const {
  size_t bit_num, max_index;
  if (bit_num_ < bv.bit_num_) {
    bit_num = bit_num_;
    max_index = bits_.size() - 1;
  } else {
    bit_num = bv.bit_num_;
    max_index = bv.bits_.size() - 1;
  }
  if (bit_num == 0) {
    return 0;
  }

  const uint64_t* a = &bits_[0];
  const uint64_t* a_back = a + max_index;
  const uint64_t* b = &bv.bits_[0];

  uint64_t heads_match = 0;
  while (a != a_back) {
    heads_match += pop_count(~(*a++ ^ *b++));
  }
  const uint64_t tail_match = (bit_num - 1) % BLOCKSIZE + 1
      - pop_count(*a ^ *b);
  return heads_match + tail_match + (bit_num_ + bv.bit_num_ - 2 * bit_num);
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
