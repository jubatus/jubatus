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
#include <string>
#include <gtest/gtest.h>
#include <pficommon/text/json.h>
#include "converter_config.hpp"
#include "datum.hpp"
#include "datum_to_fv_converter.hpp"
#include "exception.hpp"

using std::string;
using pfi::data::optional;

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(converter_config, config) {
  try {
#ifdef HAVE_RE2
    std::ifstream ifs("./test_input/config.json");
#else
    std::ifstream ifs("./test_input/config_wo_re2.json");
#endif
    converter_config config;
    ifs >> pfi::text::json::via_json(config);

    datum_to_fv_converter conv;
    initialize_converter(config, conv);

    datum d;
    d.string_values_.push_back(std::make_pair("user/name", "Taro Yamada"));
    d.string_values_.push_back(std::make_pair("user/text", "hoge fuga <foo>"));

    d.num_values_.push_back(std::make_pair("user/id", 1000));
    d.num_values_.push_back(std::make_pair("user/age", 20));

    sfv_t f;
    conv.convert(d, f);

    sfv_t exp;
    exp.push_back(std::make_pair("user/name$Taro Yamada@str#bin/bin", 1.));
#ifdef HAVE_RE2
    // only when re2 is enabled, detagging filter works
    exp.push_back(
        std::make_pair("user/text-detagged$hoge fuga @str#bin/bin", 1.));
#endif
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

TEST(converter_config, hash) {
  converter_config config;
  num_rule r = {"*", optional<string>(), "str"};
  config.num_rules.push_back(r);
  config.hash_max_size = 1;

  datum_to_fv_converter conv;
  initialize_converter(config, conv);

  datum d;
  d.num_values_.push_back(std::make_pair("age", 10));

  sfv_t f;
  conv.convert(d, f);

  EXPECT_EQ("0", f[0].first);
}

TEST(converter_config, hash_negative) {
  converter_config config;
  config.hash_max_size = 0;
  datum_to_fv_converter conv;

  EXPECT_THROW(initialize_converter(config, conv), converter_exception);
}

TEST(make_fv_converter, empty) {
  EXPECT_THROW(make_fv_converter(""), fv_converter::converter_exception);
}

TEST(make_fv_converter, invalid_config_json) {
  EXPECT_THROW(make_fv_converter("{"), fv_converter::converter_exception);
}

TEST(make_fv_converter, config_json_parse_error) {
  EXPECT_THROW(make_fv_converter("AA"), fv_converter::converter_exception);
}

TEST(make_fv_converter, config_cast_error) {
  EXPECT_THROW(make_fv_converter("{}"), fv_converter::converter_exception);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
