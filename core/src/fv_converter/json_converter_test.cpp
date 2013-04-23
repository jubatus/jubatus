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
#include <vector>
#include <gtest/gtest.h>
#include <pficommon/text/json.h>
#include "datum.hpp"
#include "json_converter.hpp"

using pfi::text::json::json;

namespace jubatus {
namespace core {
namespace fv_converter {

json MakeJson(const std::string& string) {
  std::istringstream iss(string);
  pfi::text::json::json json;
  iss >> json;
  return json;
}

void TestEquals(
    const std::string& json_string,
    std::vector<std::pair<std::string, std::string> > expected_strings,
    std::vector<std::pair<std::string, double> > expected_nums) {
  std::sort(expected_strings.begin(), expected_strings.end());
  std::sort(expected_nums.begin(), expected_nums.end());
  datum actual;
  json json = MakeJson(json_string);
  json_converter::convert(json, actual);
  std::sort(actual.string_values_.begin(), actual.string_values_.end());
  std::sort(actual.num_values_.begin(), actual.num_values_.end());
  ASSERT_EQ(expected_strings, actual.string_values_);
  ASSERT_EQ(expected_nums, actual.num_values_);
}

TEST(json_converter, empty) {
  std::vector<std::pair<std::string, std::string> > strings;
  std::vector<std::pair<std::string, double> > nums;

  TestEquals("{}", strings, nums);
  TestEquals("[]", strings, nums);
}

TEST(json_converter, number) {
  std::vector<std::pair<std::string, std::string> > strings;
  std::vector<std::pair<std::string, double> > nums;

  nums.push_back(std::make_pair("/val1", 10.));
  nums.push_back(std::make_pair("/val2", 0.5));

  TestEquals("{ \"val1\": 10,  \"val2\": 0.5 }", strings, nums);
}

TEST(json_converter, string) {
  std::vector<std::pair<std::string, std::string> > strings;
  std::vector<std::pair<std::string, double> > nums;

  strings.push_back(std::make_pair("/val1", "hoge"));
  strings.push_back(std::make_pair("/val2", ""));

  TestEquals("{ \"val1\": \"hoge\",  \"val2\": \"\" }", strings, nums);
}

TEST(json_converter, bool) {
  std::vector<std::pair<std::string, std::string> > strings;
  std::vector<std::pair<std::string, double> > nums;

  nums.push_back(std::make_pair("/val1", 1));
  nums.push_back(std::make_pair("/val2", 0));

  TestEquals("{ \"val1\": true,  \"val2\": false }", strings, nums);
}

TEST(json_converter, null) {
  std::vector<std::pair<std::string, std::string> > strings;
  std::vector<std::pair<std::string, double> > nums;

  strings.push_back(std::make_pair("/val", "null"));

  TestEquals("{ \"val\": null }", strings, nums);
}

TEST(json_converter, array) {
  std::vector<std::pair<std::string, std::string> > strings;
  std::vector<std::pair<std::string, double> > nums;

  nums.push_back(std::make_pair("[0]", 1.));
  nums.push_back(std::make_pair("[1]", 2.));
  nums.push_back(std::make_pair("[2]", 3.));

  TestEquals("[1, 2, 3]", strings, nums);

  nums.clear();
  nums.push_back(std::make_pair("/key[0]", 1));
  nums.push_back(std::make_pair("/key[1]", 2));
  nums.push_back(std::make_pair("/key[2]", 3));
  TestEquals("{\"key\": [1, 2, 3]}", strings, nums);
}

TEST(json_converter, simple) {
  std::vector<std::pair<std::string, std::string> > strings;
  std::vector<std::pair<std::string, double> > nums;
  strings.push_back(std::make_pair("/str", "string"));
  nums.push_back(std::make_pair("/num", 10.));

  TestEquals("{ \"str\": \"string\", \"num\": 10 }", strings, nums);
}

TEST(json_converter, object) {
  std::vector<std::pair<std::string, std::string> > strings;
  std::vector<std::pair<std::string, double> > nums;
  strings.push_back(std::make_pair("/text", "Hello"));
  strings.push_back(std::make_pair("/user/name", "Taro"));
  nums.push_back(std::make_pair("/user/age", 20));

  TestEquals(
      "{ \"text\": \"Hello\", \"user\": { \"name\": \"Taro\", \"age\": 20 } }",
      strings, nums);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
