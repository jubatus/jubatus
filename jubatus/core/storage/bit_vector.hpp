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

#ifndef JUBATUS_CORE_STORAGE_BIT_VECTOR_HPP_
#define JUBATUS_CORE_STORAGE_BIT_VECTOR_HPP_

#include <stdint.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <pficommon/data/serialization.h>

namespace jubatus {
namespace core {
namespace storage {

class bit_vector {
 public:
  bit_vector();
  ~bit_vector();

  bool operator==(const bit_vector& v) const;

  void resize_and_clear(uint64_t bit_num);
  void set_bit(uint64_t pos);
  uint64_t calc_hamming_similarity(const bit_vector& bv) const;

  static uint64_t pop_count(uint64_t r) {
    r = (r & 0x5555555555555555ULL) + ((r >> 1) & 0x5555555555555555ULL);
    r = (r & 0x3333333333333333ULL) + ((r >> 2) & 0x3333333333333333ULL);
    r = (r + (r >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
    r = r + (r >> 8);
    r = r + (r >> 16);
    r = r + (r >> 32);
    return (uint64_t)(r & 0x7f);
  }

  uint64_t bit_num() const {
    return bit_num_;
  }

  void debug_print(std::ostream& os) const {
    for (uint64_t i = 0; i < bit_num_; ++i) {
      if ((bits_[i / 64] >> (i % 64)) & 1LLU) {
        os << "1";
      } else {
        os << "0";
      }
    }
  }

  void swap(bit_vector& v) {
    bits_.swap(v.bits_);
    std::swap(bit_num_, v.bit_num_);
  }

 private:
  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(bits_) & MEMBER(bit_num_);
  }

  std::vector<uint64_t> bits_;
  uint64_t bit_num_;
};

}  // namespace storage
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_STORAGE_BIT_VECTOR_HPP_
