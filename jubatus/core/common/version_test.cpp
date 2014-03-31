// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "version.hpp"


namespace jubatus {
namespace core {
namespace storage {

TEST(version, initial_zero) {
  version v;
  ASSERT_EQ(0u, v.get_number());
}

TEST(version, increment) {
  version v;
  v.increment();
  ASSERT_EQ(1u, v.get_number());
}

TEST(version, compare) {
  version v1, v2;
  ASSERT_EQ(v1, v2);
  ASSERT_LE(v2, v1);
  ASSERT_GE(v1, v2);
  v1.increment();
  ASSERT_NE(v1, v2);
  ASSERT_LT(v2, v1);
  ASSERT_LE(v2, v1);
  ASSERT_GT(v1, v2);
  ASSERT_GE(v1, v2);
  v2.increment();
  ASSERT_EQ(v1, v2);
  ASSERT_LE(v1, v2);
  ASSERT_GE(v1, v2);
}

namespace {
std::string to_s(const version& v) {
  std::stringstream ss;
  ss << v;
  return ss.str();
}
}

TEST(version, output) {
  version v;
  ASSERT_EQ("0th", to_s(v));
  v.increment();
  ASSERT_EQ("1st", to_s(v));
  v.increment();
  ASSERT_EQ("2nd", to_s(v));
  v.increment();
  ASSERT_EQ("3rd", to_s(v));
  v.increment();
  ASSERT_EQ("4th", to_s(v));
  v.increment();
  ASSERT_EQ("5th", to_s(v));
  v.increment();
  ASSERT_EQ("6th", to_s(v));
  v.increment();
  ASSERT_EQ("7th", to_s(v));
  v.increment();
  ASSERT_EQ("8th", to_s(v));
  v.increment();
  ASSERT_EQ("9th", to_s(v));
  v.increment();
  ASSERT_EQ("10th", to_s(v));
  v.increment();
  ASSERT_EQ("11th", to_s(v));
  v.increment();
  ASSERT_EQ("12th", to_s(v));
  v.increment();
  ASSERT_EQ("13th", to_s(v));
  for (; v.get_number() < 100; v.increment());
  ASSERT_EQ("100th", to_s(v));
  v.increment();
  ASSERT_EQ("101st", to_s(v));
  v.increment();
  ASSERT_EQ("102nd", to_s(v));
  v.increment();
  ASSERT_EQ("103rd", to_s(v));
  v.increment();
  ASSERT_EQ("104th", to_s(v));
  for (; v.get_number() < 1000; v.increment());
  ASSERT_EQ("1000th", to_s(v));
  v.increment();
  ASSERT_EQ("1001st", to_s(v));
  v.increment();
  ASSERT_EQ("1002nd", to_s(v));
  v.increment();
  ASSERT_EQ("1003rd", to_s(v));
  v.increment();
  ASSERT_EQ("1004th", to_s(v));
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
