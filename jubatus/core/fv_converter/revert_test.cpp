// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <algorithm>
#include <string>
#include <utility>
#include <gtest/gtest.h>
#include "datum.hpp"
#include "revert.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(revert_num_value, str) {
  std::pair<std::string, float> value;
  ASSERT_TRUE(revert_num_value(std::make_pair("age@str$25", 1), value));
  EXPECT_EQ("age", value.first);
  EXPECT_EQ(25.f, value.second);
}

TEST(revert_num_value, num) {
  std::pair<std::string, float> value;
  ASSERT_TRUE(revert_num_value(std::make_pair("age@num", 25.f), value));
  EXPECT_EQ("age", value.first);
  EXPECT_EQ(25.f, value.second);
}

TEST(revert_num_value, error) {
  std::pair<std::string, float> value;
  ASSERT_FALSE(revert_num_value(std::make_pair("age", 1), value));
  ASSERT_FALSE(revert_num_value(std::make_pair("age@log", 1), value));
  ASSERT_FALSE(revert_num_value(std::make_pair("age@str$hoge", 1), value));
}

TEST(revert_string_value, trivial) {
  std::pair<std::string, std::string> value;
  ASSERT_TRUE(revert_string_value(
      std::make_pair("/name$doc1@str#bin/bin", 1.0), value));
  EXPECT_EQ("/name", value.first);
  EXPECT_EQ("doc1", value.second);
}

TEST(revert_string_value, error) {
  std::pair<std::string, std::string> value;
  EXPECT_FALSE(revert_string_value(std::make_pair("/age$1@str", 1.0), value));
  EXPECT_FALSE(revert_string_value(
      std::make_pair("/age$1#bin/bin", 1.0), value));
  EXPECT_FALSE(revert_string_value(
      std::make_pair("/age@str#bin/bin", 1.0), value));
  EXPECT_FALSE(revert_string_value(
      std::make_pair("/name$hoge@space#bin/bin", 1.0), value));
}

TEST(revert_feature, trivial) {
  fv_converter::datum data;
  sfv_t fv;

  fv.push_back(std::make_pair("name$doc1@str#bin/bin", 1.0));
  fv.push_back(std::make_pair("message$hello@space#bin/bin", 1.0));
  fv.push_back(std::make_pair("age@num", 25.f));
  fv.push_back(std::make_pair("id@str$12345", 1.0));
  fv.push_back(std::make_pair("age@log", 3.0));

  revert_feature(fv, data);

  ASSERT_EQ(1u, data.string_values_.size());
  EXPECT_EQ("name", data.string_values_[0].first);
  EXPECT_EQ("doc1", data.string_values_[0].second);
  ASSERT_EQ(2u, data.num_values_.size());
  std::sort(data.num_values_.begin(), data.num_values_.end());
  EXPECT_EQ("age", data.num_values_[0].first);
  EXPECT_EQ(25.f, data.num_values_[0].second);
  EXPECT_EQ("id", data.num_values_[1].first);
  EXPECT_EQ(12345, data.num_values_[1].second);
}


TEST(revert_feature, dollar_mark_onvalue) {
  fv_converter::datum data;
  sfv_t fv;

  fv.push_back(std::make_pair("name$do$c1@str#bin/bin", 1.0));

  revert_feature(fv, data);

  ASSERT_EQ(1u, data.string_values_.size());
  EXPECT_EQ("name", data.string_values_[0].first);
  EXPECT_EQ("do$c1", data.string_values_[0].second);
  ASSERT_EQ(0, data.num_values_.size());
}


}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
