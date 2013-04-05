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

#ifndef BIT_VECTOR_HPP
#define BIT_VECTOR_HPP

#include <stdint.h>
#include <assert.h>

#include <iostream>
#include <string>
#include <vector>

#include <msgpack.hpp>
#include <pficommon/lang/cast.h>
#include <pficommon/data/serialization.h>
#include "../../common/exception.hpp"
#include "../storage_exception.hpp"

namespace jubatus {
namespace table {

namespace detail {
template <typename T, size_t N> struct bitcount_impl;
template <typename T>
struct bitcount_impl<T, 1> {
  static size_t call(T bits) {
    bits = (bits & 0x55) + (bits >> 1 & 0x55);
    bits = (bits & 0x33) + (bits >> 2 & 0x33);
    return (bits & 0x0f) + (bits >> 4 & 0x0f);
  }
};
template <typename T>
struct bitcount_impl<T, 2> {
  static size_t call(T bits) {
    bits = (bits & 0x5555U) + (bits >> 1 & 0x5555U);
    bits = (bits & 0x3333U) + (bits >> 2 & 0x3333U);
    bits = (bits & 0x0f0fU) + (bits >> 4 & 0x0f0fU);
    return (bits & 0x00ffU) + (bits >> 8 & 0x00ffU);
  }
};
template <typename T>
struct bitcount_impl<T, 4> {
  static size_t call(T bits) {
    bits = (bits & 0x55555555LU) + (bits >> 1 & 0x55555555LU);
    bits = (bits & 0x33333333LU) + (bits >> 2 & 0x33333333LU);
    bits = (bits & 0x0f0f0f0fLU) + (bits >> 4 & 0x0f0f0f0fLU);
    bits = (bits & 0x00ff00ffLU) + (bits >> 8 & 0x00ff00ffLU);
    return (bits & 0x0000ffffLU) + (bits >>16 & 0x0000ffffLU);
  }
};
template <typename T>
struct bitcount_impl<T, 8> {
  static size_t call(T bits) {
    bits = (bits & 0x5555555555555555LLU) + (bits >> 1 & 0x5555555555555555LLU);
    bits = (bits & 0x3333333333333333LLU) + (bits >> 2 & 0x3333333333333333LLU);
    bits = (bits & 0x0f0f0f0f0f0f0f0fLLU) + (bits >> 4 & 0x0f0f0f0f0f0f0f0fLLU);
    bits = (bits & 0x00ff00ff00ff00ffLLU) + (bits >> 8 & 0x00ff00ff00ff00ffLLU);
    bits = (bits & 0x0000ffff0000ffffLLU) + (bits >>16 & 0x0000ffff0000ffffLLU);
    return (bits & 0x00000000ffffffffLLU) + (bits >>32 & 0x00000000ffffffffLLU);
  }
};
template <typename T>
inline size_t bitcount(T bits) {
  return bitcount_impl<T, sizeof(T)>::call(bits);
}
} /* detail */

class bit_vector_unmatch_exception
  : public jubatus::exception::jubaexception<bit_vector_unmatch_exception> {
 public:
  explicit bit_vector_unmatch_exception(const std::string &msg)
    : msg_(msg) {}
  ~bit_vector_unmatch_exception() throw() {}
  const char* what() const throw() {return msg_.c_str();}

 private:
  std::string msg_;
};

template <typename bit_base>
struct bit_vector_base {
  typedef bit_vector_base<bit_base> bit_vector_t;
  static const size_t BASE_BITS = sizeof(bit_base) * 8;
  static const size_t BLOCKSIZE = sizeof(bit_base);

  bit_vector_base(void* bits, size_t bit_num)
    :bits_(reinterpret_cast<bit_base*>(bits)),
     bit_num_(bit_num),
     own_(false) {
  }
  explicit bit_vector_base(int bit_num)
    :bits_(NULL), bit_num_(bit_num), own_(false) { }

bit_vector_base(const bit_vector_t& orig)
    :bits_(NULL), bit_num_(orig.bit_num_), own_(true)
  {
    if (orig.bits_ == NULL) {
      return;
    }
    alloc_memory();
    memcpy(bits_, orig.bits_, used_bytes());
  }

  bool operator==(const bit_vector_t& rhs) const {
    if (bit_num_ != rhs.bit_num_) {
      return false;
    }
    if (is_empty() && rhs.is_empty()) {
      return true;
    }
    if (is_empty() && !rhs.is_empty()) {
      return false;
    }
    if (!is_empty() && rhs.is_empty()) {
      return false;
    }
    return memcmp(bits_, rhs.bits_, used_bytes()) == 0;
  }

  // deep copy (In case not own memory, it alloc memory)
  bit_vector_t& operator=(const bit_vector_t& orig) {
    if (bit_num_ != orig.bit_num_) {
      throw bit_vector_unmatch_exception("failed copy bit vector from "
          + pfi::lang::lexical_cast<std::string>(orig.bit_num_)
          + " to "
          + pfi::lang::lexical_cast<std::string>(bit_num_));
    }
    if (bits_ == NULL) {
      alloc_memory();
    }
    if (orig.bits_ == NULL) {
      memset(bits_, 0, used_bytes());
    } else {
      memcpy(bits_, orig.bits_, used_bytes());
    }
    return *this;
  }
  template <typename T>
  bit_vector_t& operator=(const T& orig) {
    throw type_unmatch_exception("failed copy bit vector from ");
    return *this;
  }

  void clear_bit(size_t pos) {
    if (bits_ == NULL) {return;}
    bits_[pos / BASE_BITS] &= ~(1LLU << (pos % BASE_BITS));
  }
  void set_bit(size_t pos) {
    if (bits_ == NULL) { alloc_memory(); }
    if (static_cast<size_t>(bit_num_) < pos) {
      throw
        bit_vector_unmatch_exception("set_bit(): invalid posison "
                                     + pfi::lang::lexical_cast<std::string>(pos)
                                     + " for length: "
                                     + pfi::lang::lexical_cast<std::string>(
                                           bit_num_));
    }
    bits_[pos / BASE_BITS] |= (1LLU << (pos % BASE_BITS));
  }
  void reverse_bit(size_t pos) {
    if (bits_ == NULL) { alloc_memory(); }
    if (bit_num_ < pos) {
      throw
        bit_vector_unmatch_exception("reverse_bit(): invalid posison "
                                     + pfi::lang::lexical_cast<std::string>(pos)
                                     + " for length: "
                                     + pfi::lang::lexical_cast<std::string>(
                                           bit_num_));
    }
    bits_[pos / BASE_BITS] ^= (1LLU << (pos % BASE_BITS));
  }

  bool get_bit(size_t pos) const {
    if (bits_ == NULL) { return false; }
    return bits_[pos / BASE_BITS] & (1LLU << (pos % BASE_BITS));
  }
  bool is_empty() const {
    return bit_count() == 0;
  }

  void clear() {
    for (size_t i = 0; i < bit_num_; ++i) {
      clear_bit(i);
    }
  }
  uint64_t calc_hamming_similarity(const bit_vector_t& bv) const {
    return bit_num() - calc_hamming_distance(bv);
  }
  uint64_t calc_hamming_distance(const bit_vector_t& bv) const {
    if (bit_num() != bv.bit_num()) {
      throw bit_vector_unmatch_exception("calc_hamming_similarity(): "
                                         "bit_vector length unmatch! "
                                         + pfi::lang::lexical_cast<std::string>(
                                               bit_num()) +
                                         " with "
                                         + pfi::lang::lexical_cast<std::string>(
                                               bv.bit_num()));
    }
    if (is_empty() && bv.is_empty()) {
      return 0;
    } else if (is_empty()) {
      return bv.bit_count();
    } else if (bv.is_empty()) {
      return bit_count();
    }
    size_t match_num = 0;
    for (size_t i = 0; i < used_bytes() / sizeof(bit_base); ++i) {
      match_num += detail::bitcount(bits_[i] ^ bv.bits_[i]);
    }
    return match_num;
  }
  size_t bit_count() const {
    if (bits_ == NULL) {
      return 0;
    }
    size_t result = 0;
    for (int64_t i = (used_bytes() / sizeof(bit_base)) - 1; i >= 0; --i) {
      result += detail::bitcount(bits_[i]);
    }
    return result;
  }
  static uint64_t pop_count(uint64_t r) {
    r = (r & 0x5555555555555555ULL) +
      ((r >> 1) & 0x5555555555555555ULL);
    r = (r & 0x3333333333333333ULL) +
      ((r >> 2) & 0x3333333333333333ULL);
    r = (r + (r >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
    r = r + (r >>  8);
    r = r + (r >> 16);
    r = r + (r >> 32);
    return (uint64_t)(r & 0x7f);
  }
  static size_t pop_count(uint8_t r) {
    r = (r & 0x55U) +
      ((r >> 1) & 0x55U);
    r = (r & 0x33U) +
      ((r >> 2) & 0x33U);
    r = (r + (r >> 4)) & 0x0fU;
    return (size_t)(r & 0x7f);
  }

  bit_base* raw_data_unsafe() {
    return bits_;
  }
  const bit_base* raw_data_unsafe() const {
    return bits_;
  }
  size_t bit_num() const {
    return bit_num_;
  }
  size_t used_bytes() const {
    return memory_size(bit_num_);
  }
  static size_t memory_size(size_t bit_width) {
    return ((((bit_width + 7) / 8) + BLOCKSIZE - 1) / BLOCKSIZE) * BLOCKSIZE;
  }
  ~bit_vector_base() {
    if (bits_ && own_) {
      release();
      return;
    }
  }

  void debug_print(std::ostream& os) const {
    if (bits_ == NULL) { os << "(unallocated)"; return;}
    for (uint64_t i = 0; i < used_bytes() * 8; ++i) {
      if ((bits_[i / (sizeof(bit_base) * 8)] >> (i % (sizeof(bit_base) * 8)))
            & 1LLU) {
        os << "1";
      } else {
        os << "0";
      }
    }
  }
  friend std::ostream& operator<<(std::ostream& os, const bit_vector_t& bv) {
    bv.debug_print(os);
    return os;
  }
  void status(std::ostream& os) const {
    os << "status():";
    if (bits_ == NULL) { os << "(unallocated)" << " owns_" << own_; return;}
    os << (own_ ? "[own]" : "[other]") << std::endl;
  }

  template <typename packable>
  void pack(packable& buffer) const {
    const std::string tmp(reinterpret_cast<const char*>(bits_), used_bytes());
    msgpack::pack(buffer, tmp);
  }
  void alloc_memory() {
    assert(!own_);
    bits_ = new bit_base[used_bytes()];
    own_ = true;
    memset(bits_, 0, used_bytes());
  }

 private:
  bool release() {
    if (bits_ && own_) {
      delete[] bits_;
      own_ = false;
      bits_ = NULL;
      return true;
    }
    return false;
  }

  // deep copy
  void duplicate(const bit_vector_t& orig) {
    if (bit_num_ != orig.bit_num_) {
      throw bit_vector_unmatch_exception("failed copy bit vector from "
                                         + pfi::lang::lexical_cast<std::string>(
                                               orig.bit_num_)
                                         + " to "
                                         + pfi::lang::lexical_cast<std::string>(
                                               bit_num_));
    }
    if (!own_) {
      alloc_memory();
    }
    memcpy(bits_, orig.bits_, used_bytes());
  }

  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(bits_)
      & MEMBER(bit_num);
  }

  bit_base* bits_;
  const size_t bit_num_;
  bool own_;
};
typedef bit_vector_base<uint64_t> bit_vector;

template<typename bit_base>
class const_bit_vector_base : private bit_vector_base<bit_base> {
  typedef bit_vector_base<bit_base> base_bit_vector_t;

 public:
  const_bit_vector_base(const void* bits, int bit_num)
    :base_bit_vector_t(const_cast<void*>(bits), bit_num) {}
  explicit const_bit_vector_base(const base_bit_vector_t& orig)
    :base_bit_vector_t(const_cast<base_bit_vector_t&>(orig)) {}
  using base_bit_vector_t::debug_print;
  using base_bit_vector_t::calc_hamming_similarity;
  using base_bit_vector_t::bit_count;
  using base_bit_vector_t::operator==;
  using base_bit_vector_t::get_bit;
  using base_bit_vector_t::status;
  using base_bit_vector_t::bit_num;
  using base_bit_vector_t::used_bytes;
  friend std::ostream& operator<<(
      std::ostream& os,
      const base_bit_vector_t& bv) {
    bv.debug_print(os);
    return os;
  }
};
typedef const_bit_vector_base<uint64_t> const_bit_vector;

}  // namespace column
}  // namespace jubatus
#endif  // BIT_VECTOR_HPP
