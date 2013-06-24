// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_COMMON_BYTE_BUFFER_HPP_
#define JUBATUS_CORE_COMMON_BYTE_BUFFER_HPP_

#include <stdint.h>
#include <vector>
#include <cstring>
#include <msgpack.hpp>
#include <pficommon/lang/shared_ptr.h>

namespace jubatus {
namespace core {
namespace common {

class byte_buffer {
 public:
  byte_buffer() {
  }

  explicit byte_buffer(size_t size)
      : buf_(new std::vector<char>(size)) {
  }

  byte_buffer(const void* ptr, size_t size) {
    const char* const first = static_cast<const char*>(ptr);
    buf_.reset(new std::vector<char>(first, first+size));
  }

  // following member functions are implicily defined:
  //   byte_buffer(const byte_buffer& b) = default;
  //   byte_buffer& operator=(const byte_buffer& b) = default;
  //   ~byte_buffer() = default;

  void swap(byte_buffer& other) {
    this->buf_.swap(other.buf_);
    // pfi::lang::shared_ptr provides no non-member swap;
    // `swap(this->buf_, other.buf_);' may be inefficient.
    // when pfi::lang::shared_ptr provide non-member swap,
    // this function should be rewritten with it
    // because if new data members are added to
    // pfi::lang::shared_ptr, member function swap
    // (currently derived from base) may cause object slicing.
  }

  void assign(const void* ptr, size_t size) {
    if (buf_.unique()) {
      const char* const first = static_cast<const char*>(ptr);
      buf_->assign(first, first + size);
    } else {
      byte_buffer(ptr, size).swap(*this);
    }
  }

  const char* ptr() const {
    if (buf_ && !buf_->empty()) {
      return &(*buf_)[0];
      // `buf_->data()' is much better (C++11 feature)
    } else {
      return NULL;
    }
  }

  char* ptr() {
    return const_cast<char*>(
      const_cast<const byte_buffer*>(this)->ptr());
  }

  size_t size() const {
    if (buf_) {
      return buf_->size();
    } else {
      return 0;
    }
  }

 private:
  pfi::lang::shared_ptr<std::vector<char> > buf_;
};

inline void swap(byte_buffer& one, byte_buffer& another) {  // NOLINT
  one.swap(another);
}

}  // namespace common
}  // namespace core
}  // namespace jubatus

namespace msgpack {

inline jubatus::core::common::byte_buffer& operator>>(
    object o,
    jubatus::core::common::byte_buffer& b) {
  if (o.type != type::RAW) {
    throw type_error();
  }

  b.assign(o.via.raw.ptr, o.via.raw.size);
  return b;
}

template<typename Stream>
inline packer<Stream>& operator<<(
    packer<Stream>& o,
    const jubatus::core::common::byte_buffer& b) {
  o.pack_raw(b.size());
  o.pack_raw_body(b.ptr(), b.size());
  return o;
}

inline void operator<<(
    object::with_zone& o,
    const jubatus::core::common::byte_buffer& b) {
  o.type = type::RAW;
  char* ptr = static_cast<char*>(o.zone->malloc(b.size()));
  o.via.raw.ptr = ptr;
  o.via.raw.size = static_cast<uint32_t>(b.size());
  std::memcpy(ptr, b.ptr(), b.size());
}

inline void operator<<(
    object& o,
    const jubatus::core::common::byte_buffer& b) {
  o.type = type::RAW;
  o.via.raw.ptr = b.ptr();
  o.via.raw.size = static_cast<uint32_t>(b.size());
}

}  // namespace msgpack

#endif  // JUBATUS_CORE_COMMON_BYTE_BUFFER_HPP_
