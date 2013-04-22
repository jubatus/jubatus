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

#include <functional>
#include <vector>
#include <gtest/gtest.h>
#include "fixed_size_heap.hpp"

using std::greater;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {

TEST(fixed_size_heap, empyt) {
  fixed_size_heap<int> h(10);
  EXPECT_EQ(0u, h.size());
  EXPECT_EQ(10u, h.get_max_size());

  vector<int> v;
  h.get_sorted(v);
  EXPECT_TRUE(v.empty());
}

TEST(fixed_size_heap, small) {
  fixed_size_heap<int> h(10);
  h.push(1);
  h.push(5);
  h.push(3);

  EXPECT_EQ(3u, h.size());

  vector<int> v;
  h.get_sorted(v);
  EXPECT_EQ(1, v[0]);
  EXPECT_EQ(3, v[1]);
  EXPECT_EQ(5, v[2]);
}

TEST(fixed_size_heap, large) {
  fixed_size_heap<int> h(3);
  for (int i = 0; i < 10; ++i)
  h.push(i * 7 % 10);

  EXPECT_EQ(3u, h.size());

  vector<int> v;
  h.get_sorted(v);
  EXPECT_EQ(0, v[0]);
  EXPECT_EQ(1, v[1]);
  EXPECT_EQ(2, v[2]);
}

TEST(fixed_size_heap, reverse) {
  fixed_size_heap<int, greater<int> > h(3);
  for (int i = 0; i < 10; ++i)
  h.push(i * 7 % 10);

  EXPECT_EQ(3u, h.size());

  vector<int> v;
  h.get_sorted(v);
  EXPECT_EQ(9, v[0]);
  EXPECT_EQ(8, v[1]);
  EXPECT_EQ(7, v[2]);
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
