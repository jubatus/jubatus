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

#include <utility>
#include <gtest/gtest.h>
#include "vector_util.hpp"

using std::make_pair;

namespace jubatus {

TEST(sort_and_merge, empty) {
  sfv_t v;
  sort_and_merge(v);
  EXPECT_TRUE(v.empty());
}

TEST(sort_and_merge, trivial) {
  sfv_t v;
  v.push_back(make_pair("f4", 1.0));
  v.push_back(make_pair("f2", 2.0));
  v.push_back(make_pair("f4", 3.0));
  sort_and_merge(v);
  ASSERT_EQ(2u, v.size());
  EXPECT_EQ("f2", v[0].first);
  EXPECT_EQ(2.0, v[0].second);
  EXPECT_EQ("f4", v[1].first);
  EXPECT_EQ(4.0, v[1].second);
}

}  // namespace jubatus
