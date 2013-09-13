// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_COMMON_BIG_ENDIAN_HPP_
#define JUBATUS_CORE_COMMON_BIG_ENDIAN_HPP_

#include <stdint.h>
#include <cstddef>

namespace jubatus {
namespace core {
namespace common {

template<class UInt>
void write_big_endian_(UInt x, char* buf) {
  for (int n = sizeof(x) - 1; n >= 0; --n) {
    *(buf++) = (x >> (n * 8)) & 0xff;
  }
}

inline void write_big_endian(uint32_t x, char* buf) {
  common::write_big_endian_(x, buf);
}
inline void write_big_endian(uint64_t x, char* buf) {
  common::write_big_endian_(x, buf);
}

template<class UInt>
UInt read_big_endian(const char* buf) {
  UInt x = 0;
  for (size_t i = 0; i < sizeof(x); ++i) {
    x = (x << 8) | (buf[i] & 0xff);
  }
  return x;
}

}  // namespace common
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_COMMON_BIG_ENDIAN_HPP_
