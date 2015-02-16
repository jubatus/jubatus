// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include "crc32.hpp"

namespace jubatus {
namespace server {
namespace common {

namespace {

class crc32_calculator {
 public:
  crc32_calculator() {
    for (int i = 0; i < 256; ++i) {
      uint32_t c = i;
      for (int j = 0; j < 8; ++j) {
        c = (c >> 1) ^ (c & 1 ? 0xEDB88320 : 0);
      }
      crc_table_[i] = c;
    }
  }

  uint32_t operator()(const char* buf, size_t size, uint32_t crc) const {
    crc ^= 0xFFFFFFFF;
    for (size_t i = 0; i < size; ++i) {
      size_t idx = (buf[i] ^ crc) & 0xFF;
      crc = (crc >> 8) ^ crc_table_[idx];
    }
    return crc ^ 0xFFFFFFFF;
  }

 private:
  uint32_t crc_table_[256];
};

const crc32_calculator calc_;

}  // namespace

uint32_t calc_crc32(const char* data, size_t size, uint32_t crc) {
  return calc_(data, size, crc);
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
