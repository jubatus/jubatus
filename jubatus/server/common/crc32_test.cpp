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

#include <cstdlib>
#include <gtest/gtest.h>
#include "crc32.hpp"

namespace jubatus {
namespace server {
namespace common {

TEST(calc_crc32, simple) {
  EXPECT_EQ(0u, calc_crc32("", 0));
  EXPECT_EQ(0x41918955u, calc_crc32("jubatus", 7));
}

TEST(calc_crc32, piecewise) {
  std::srand(testing::UnitTest::GetInstance()->random_seed());

  char data[128];
  for (size_t i = 0; i < sizeof(data); ++i) {
    data[i] = static_cast<int>((std::rand() / (RAND_MAX + 1.0)) * 256);
  }

  uint32_t crc_expected = calc_crc32(data, sizeof(data));

  uint32_t crc_actual = 0;
  for (size_t i = 0; i < sizeof(data); ) {
    size_t n = static_cast<int>((std::rand() / (RAND_MAX + 1.0)) * 32);
    if (i + n > sizeof(data)) {
      n = sizeof(data) - i;
    }
    crc_actual = calc_crc32(data + i, n, crc_actual);
    i += n;
  }

  EXPECT_EQ(crc_expected, crc_actual);
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
