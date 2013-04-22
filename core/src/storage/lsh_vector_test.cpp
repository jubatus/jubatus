// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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
#include <vector>
#include <gtest/gtest.h>
#include "lsh_vector.hpp"

using std::string;
using std::stringstream;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {

lsh_vector make_vector(const string& str) {
  stringstream ss;
  ss << str;

  vector<int> v;
  for (int n; ss >> n;) {
    v.push_back(n);
  }
  return lsh_vector(v);
}

TEST(lsh_vector, empty) {
  lsh_vector lv;
  EXPECT_EQ(0u, lv.size());
}

TEST(lsh_vector, sized) {
  lsh_vector lv(2);
  EXPECT_EQ(2u, lv.size());
}

TEST(lsh_vector, value) {
  lsh_vector lv(4);
  lv.set(0, 1);
  lv.set(1, 2);
  lv.set(2, 3);
  lv.set(3, 4);

  EXPECT_EQ(1, lv.get(0));
  EXPECT_EQ(2, lv.get(1));
  EXPECT_EQ(3, lv.get(2));
  EXPECT_EQ(4, lv.get(3));
}

TEST(lsh_vector, resize) {
  lsh_vector lv(4);
  lv.set(2, 1);

  lv.resize_and_clear(8);
  EXPECT_EQ(8u, lv.size());
  EXPECT_EQ(0, lv.get(2));
}

TEST(lsh_vector, from_vector) {
  lsh_vector lv = make_vector("1 2 3 4");
  EXPECT_EQ(4u, lv.size());
  EXPECT_EQ(1, lv.get(0));
  EXPECT_EQ(2, lv.get(1));
  EXPECT_EQ(3, lv.get(2));
  EXPECT_EQ(4, lv.get(3));
}

TEST(lsh_vector, slice) {
  const lsh_vector lv = make_vector("1 2 3 4 5 6 7 8");
  const lsh_vector expect = make_vector("3 4 5 6");
  EXPECT_EQ(expect, lv.slice(2, 4));
}

TEST(lsh_vector, cut) {
  const lsh_vector lv = make_vector("1 2 3 4 5 6 7 8");
  const lsh_vector expect = make_vector("1 2 7 8");
  EXPECT_EQ(expect, lv.cut(2, 4));
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
