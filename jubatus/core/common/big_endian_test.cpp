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

#include <cstdlib>
#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "big_endian.hpp"

namespace jubatus {
namespace core {
namespace common {

TEST(big_endian, uint32_t) {
  uint32_t x = 0x01234567u;

  char buf[sizeof(x)];
  write_big_endian(x, buf);

  EXPECT_EQ('\x01', buf[0]);
  EXPECT_EQ('\x23', buf[1]);
  EXPECT_EQ('\x45', buf[2]);
  EXPECT_EQ('\x67', buf[3]);

  uint32_t y = read_big_endian<uint32_t>(buf);
  EXPECT_EQ(x, y);
}

TEST(big_endian, uint64_t) {
  uint64_t x = 0x0123456789abcdefuL;

  char buf[sizeof(x)];
  write_big_endian(x, buf);

  EXPECT_EQ('\x01', buf[0]);
  EXPECT_EQ('\x23', buf[1]);
  EXPECT_EQ('\x45', buf[2]);
  EXPECT_EQ('\x67', buf[3]);
  EXPECT_EQ('\x89', buf[4]);
  EXPECT_EQ('\xab', buf[5]);
  EXPECT_EQ('\xcd', buf[6]);
  EXPECT_EQ('\xef', buf[7]);

  uint64_t y = read_big_endian<uint64_t>(buf);
  EXPECT_EQ(x, y);
}

}  // namespace common
}  // namespace core
}  // namespace jubatus
