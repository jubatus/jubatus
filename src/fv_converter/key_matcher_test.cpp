// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <gtest/gtest.h>

#include "key_matcher.hpp"
#include "exact_match.hpp"
#include "prefix_match.hpp"
#include "suffix_match.hpp"

#include <string>

using namespace std;
using namespace jubatus;
using namespace jubatus::fv_converter;

TEST(fv_converter, key_matcher) {
  ASSERT_TRUE(exact_match("hoge").match("hoge"));
  ASSERT_FALSE(exact_match("hoge").match("foo"));

  ASSERT_TRUE(prefix_match("ho").match("hoge"));
  ASSERT_TRUE(prefix_match("hoge").match("hoge"));
  ASSERT_TRUE(prefix_match("").match("hoge"));
  ASSERT_FALSE(prefix_match("hogehgoe").match("hoge"));

  ASSERT_TRUE(suffix_match("ge").match("hoge"));
  ASSERT_TRUE(suffix_match("hoge").match("hoge"));
  ASSERT_TRUE(suffix_match("").match("hoge"));
  ASSERT_FALSE(suffix_match("hogehgoe").match("hoge"));

}
