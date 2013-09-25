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
#include <utility>
#include <vector>
#include <iostream>
#include <gtest/gtest.h>
#include <pficommon/text/json.h>
#include "converter_config.hpp"
#include "datum.hpp"
#include "datum_to_fv_converter.hpp"
#include "exception.hpp"

using std::string;
using pfi::data::optional;

namespace jubatus {
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

TEST(converter_config, time_series_features) {
  try {
    std::ifstream ifs("./test_input/config_time_series.json");
    converter_config config;
    ifs >> pfi::text::json::via_json(config);

    datum_to_fv_converter conv;
    initialize_converter(config, conv);

    datum d;
    std::vector<std::pair<uint64_t, double> > value;
    const std::string key("car001/time_series");
    const double test[] = {0.0, 1.0, 2.0, 0.0, 0.0, 2.0, 1.0, 0.0};
    const size_t kDatums = 8;
    datum datum[kDatums];
    for (size_t i = 0; i < kDatums; ++i) {
      datum[i].num_values_.push_back(std::make_pair(key, test[i]));
    }

    sfv_t exp;
    for (int i = 0; i < 8; ++i) {
      std::stringstream ss;
      ss << key << "@log_"  << i;
      exp.push_back(
          std::make_pair(
              ss.str(),
              std::log(std::max(1.0, test[i]))));
    }
    for (int i = 0; i < 8; ++i) {
      std::stringstream ss;
      ss << key << "@num_"  << i;
      exp.push_back(
          std::make_pair(
              ss.str(),
              test[i]));
    }
    exp.push_back(std::make_pair("car001/time_series@avg", 0.75));
    exp.push_back(std::make_pair("car001/time_series@std", 0.82915619));

    const double fft_real[] = {
      6., -0.7071068, -3., 0.7071068, 0., 0.7071068, -3., -0.7071068};
    const double fft_imag[] = {
      0., -0.2928932, -3., 1.7071068, 0., -1.7071068, 3., 0.2928932};
    for (int i = 0; i < 8; ++i) {
      std::stringstream ss;
      ss << key << "@fft_real_" << i;
      exp.push_back(std::make_pair(ss.str(), fft_real[i]));
      std::stringstream ss2;
      ss2 << key << "@fft_imag_" << i;
      exp.push_back(std::make_pair(ss2.str(), fft_imag[i]));
    }

    sfv_t f;
    for (size_t i = 0; i < kDatums - 1; ++i) {
      conv.push(datum[i], "mado");
    }
    conv.convert(datum[kDatums - 1], f, "mado");

    std::sort(f.begin(), f.end());
    std::sort(exp.begin(), exp.end());
    ASSERT_EQ(exp.size(), f.size());
    for (size_t i = 0; i < f.size(); ++i) {
      ASSERT_EQ(exp[i].first, f[i].first);
      ASSERT_NEAR(exp[i].second, f[i].second, 1.0e-6);
    }
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    throw;
  } catch (const std::string& e) {
    std::cout << e << std::endl;
    throw;
  }
}


TEST(converter_config, processing_window_features) {
  try {
    std::ifstream ifs("./test_input/config_processing_window.json");
    converter_config config;
    ifs >> pfi::text::json::via_json(config);

    datum_to_fv_converter conv;
    initialize_converter(config, conv);

    datum d;
    std::vector<std::pair<uint64_t, double> > value;
    const std::string key("hoge");
    const double test[] = {0.0, 1.0, 2.0, 0.0, 0.0, 2.0, 1.0, 0.0};
    const size_t kDatums = 8;
    datum datum[kDatums];
    for (size_t i = 0; i < kDatums; ++i) {
      datum[i].num_values_.push_back(std::make_pair(key, test[i]));
    }
    float max = -100000;
    float min = 100000;
    float sum = 0;
    for (size_t i = 0; i < kDatums; ++i) {
      sum += test[i];
      max = max < test[i] ? test[i] : max;
      min = test[i] < min ? test[i] : min;
    }
    const float avg = sum / kDatums;
    float v = 0;
    for (size_t i = 0; i < kDatums; ++i) {
      v += (test[i] - avg) * (test[i] - avg);
    }
    const float var = v / kDatums;
    sfv_t exp;
    exp.push_back(std::make_pair("hoge_sum_", sum));
    exp.push_back(std::make_pair("hoge_avg_", avg));
    exp.push_back(std::make_pair("hoge_var_", var));
    exp.push_back(std::make_pair("hoge_max_", max));
    exp.push_back(std::make_pair("hoge_min_", min));

    sfv_t f;
    for (size_t i = 0; i < kDatums - 1; ++i) {
      conv.push(datum[i], "mado");
    }
    conv.convert(datum[0], f, "mado");

    ASSERT_EQ(exp.size(), f.size());
    std::sort(f.begin(), f.end());
    std::sort(exp.begin(), exp.end());
    for (size_t i = 0; i < exp.size(); ++i) {
      ASSERT_FLOAT_EQ(exp[i].second, f[i].second);
    }
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
}  // namespace jubatus
