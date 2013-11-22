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
#include <cmath>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "jubatus/util/text/json.h"
#include "converter_config.hpp"
#include "datum.hpp"
#include "datum_to_fv_converter.hpp"
#include "exception.hpp"

using std::map;
using std::string;
using jubatus::util::data::optional;

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(converter_config, config) {
  try {
    std::ifstream ifs("./test_input/config.json");
    converter_config config;
    ifs >> jubatus::util::text::json::via_json(config);

    datum_to_fv_converter conv;
    initialize_converter(config, conv);

    datum d;
    d.string_values_.push_back(std::make_pair("user/name", "Taro Yamada"));
    d.string_values_.push_back(std::make_pair("user/text", "hoge fuga <foo>"));

    d.num_values_.push_back(std::make_pair("user/id", 1000));
    d.num_values_.push_back(std::make_pair("user/age", 20));

    common::sfv_t f;
    conv.convert(d, f);

    common::sfv_t exp;
    exp.push_back(std::make_pair("user/name$Taro Yamada@str#bin/bin", 1.));

    // detagging filter
    exp.push_back(
        std::make_pair("user/text-detagged$hoge fuga @str#bin/bin", 1.));

    exp.push_back(std::make_pair("user/id@str$1000", 1.));
    exp.push_back(std::make_pair("user/age@num", 20.));
    exp.push_back(std::make_pair("user/age@log", log(20.)));
    exp.push_back(std::make_pair("user/age+1@num", 21.));

    std::sort(f.begin(), f.end());
    std::sort(exp.begin(), exp.end());
    ASSERT_EQ(exp, f);
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    throw;
  } catch (const std::string& e) {
    std::cout << e << std::endl;
    throw;
  }
}

TEST(converter_config, binary) {
  converter_config config;
  map<string, string> param;
  param["method"] = "dynamic";
  param["function"] = "create";
  param["path"] = LIBBINARY_FEATURE_SAMPLE;
  config.binary_types = map<string, param_t>();
  (*config.binary_types)["len"] = param;

  binary_rule r = {"*", optional<string>(), "len"};
  config.binary_rules = std::vector<binary_rule>();
  config.binary_rules->push_back(r);

  datum_to_fv_converter conv;
  initialize_converter(config, conv);

  datum d;
  d.binary_values_.push_back(std::make_pair("bin", "0101"));

  std::cout << "c" << std::endl;
  common::sfv_t f;
  conv.convert(d, f);
  std::cout << "d" << std::endl;

  ASSERT_EQ(1u, f.size());
  EXPECT_EQ("bin", f[0].first);
  EXPECT_EQ(4.0, f[0].second);
}

TEST(converter_config, hash) {
  converter_config config;
  num_rule r = {"*", optional<string>(), "str"};
  config.num_rules = std::vector<num_rule>();
  config.num_rules->push_back(r);
  config.hash_max_size = 1;

  datum_to_fv_converter conv;
  initialize_converter(config, conv);

  datum d;
  d.num_values_.push_back(std::make_pair("age", 10));

  common::sfv_t f;
  conv.convert(d, f);

  EXPECT_EQ("0", f[0].first);
}

TEST(converter_config, hash_negative) {
  converter_config config;
  config.hash_max_size = 0;
  datum_to_fv_converter conv;

  EXPECT_THROW(initialize_converter(config, conv), converter_exception);
}

TEST(make_fv_converter, empty_config) {
  jubatus::util::text::json::json
    js(new jubatus::util::text::json::json_object);
  converter_config config =
    jubatus::util::text::json::json_cast<converter_config>(js);
  datum_to_fv_converter conv;
  initialize_converter(config, conv);

  datum d;
  common::sfv_t f;
  conv.convert(d, f);

  EXPECT_EQ(0u, f.size());
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
