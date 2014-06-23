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

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "jubatus/util/lang/scoped_ptr.h"
#include "jubatus/util/lang/bind.h"
#include "jubatus/core/fv_converter/exception.hpp"
#include "jubatus/core/fv_converter/num_feature.hpp"
#include "jubatus/core/fv_converter/num_feature_factory.hpp"
#include "jubatus/core/fv_converter/string_feature.hpp"
#include "jubatus/core/fv_converter/string_feature_factory.hpp"

#include "jubatus/core/fv_converter/converter_config.hpp"
#include "jubatus/core/fv_converter/datum.hpp"
#include "jubatus/core/fv_converter/datum_to_fv_converter.hpp"
#include "jubatus/core/fv_converter/string_filter_factory.hpp"
#include "jubatus/core/fv_converter/num_filter_factory.hpp"
#include "jubatus/core/fv_converter/num_filter.hpp"

#include "so_factory.hpp"
#include "dynamic_num_feature.hpp"

using std::map;
using std::string;
using jubatus::util::data::optional;
using jubatus::core::common::sfv_t;
using jubatus::core::fv_converter::param_t;

using jubatus::core::fv_converter::converter_exception;
using jubatus::core::fv_converter::num_feature;
using jubatus::core::fv_converter::num_feature_factory;
using jubatus::core::fv_converter::string_feature_factory;
using jubatus::core::fv_converter::string_feature;
using jubatus::core::fv_converter::string_filter;
using jubatus::core::fv_converter::string_filter_factory;
using jubatus::core::fv_converter::num_filter;
using jubatus::core::fv_converter::num_filter_factory;

using jubatus::util::lang::bind;
using jubatus::util::lang::_1;
using jubatus::util::lang::_2;

namespace jubatus {
namespace server {
namespace fv_converter {

TEST(so_factory, num_feature_factory) {
  so_factory fa;
  num_feature_factory f(
        bind(&so_factory::create_num_feature, &fa, _1, _2));
  std::map<std::string, std::string> param;

  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["path"] = "unknown_file_name";
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["path"] = LIBNUM_FEATURE_SAMPLE;
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["function"] = "create";
  jubatus::util::lang::shared_ptr<num_feature> s(f.create("dynamic", param));

  sfv_t fv;
  s->add_feature("/path", 1, fv);

  ASSERT_EQ(1u, fv.size());
  EXPECT_EQ("/path", fv[0].first);
  EXPECT_EQ(2., fv[0].second);
}

TEST(so_factory, string_feature_factory) {
  so_factory fa;
  string_feature_factory f(
        bind(&so_factory::create_string_feature, &fa, _1, _2));
  std::map<std::string, std::string> param;
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["path"] = "unknown_file_name";
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["path"] = LIBSPLITTER_SAMPLE;
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["function"] = "create";
  jubatus::util::lang::shared_ptr<string_feature> s(f.create("dynamic", param));

  std::string d("hoge fuga");
  std::vector<jubatus::core::fv_converter::string_feature_element> bs;
  s->extract(d, bs);
  ASSERT_EQ(2u, bs.size());
  ASSERT_EQ(0u, bs[0].begin);
  ASSERT_EQ(4u, bs[0].length);
  ASSERT_EQ("hoge", bs[0].value);
  ASSERT_EQ(5u, bs[1].begin);
  ASSERT_EQ(4u, bs[1].length);
  ASSERT_EQ("fuga", bs[1].value);
}

TEST(so_factory, binary) {
  using jubatus::core::fv_converter::converter_config;
  using jubatus::core::fv_converter::datum_to_fv_converter;
  using jubatus::core::fv_converter::initialize_converter;
  using jubatus::core::fv_converter::datum;
  using jubatus::core::fv_converter::binary_rule;


  so_factory fa;
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
  initialize_converter(config, conv, &fa);

  datum d;
  d.binary_values_.push_back(std::make_pair("bin", "0101"));

  std::cout << "c" << std::endl;
  sfv_t f;
  conv.convert(d, f);
  std::cout << "d" << std::endl;

  ASSERT_EQ(1u, f.size());
  EXPECT_EQ("bin", f[0].first);
  EXPECT_EQ(4.0, f[0].second);
}


TEST(so_factory, string_filter_factory) {
  so_factory fa;
  string_filter_factory f(
        bind(&so_factory::create_string_filter, &fa, _1, _2));
  std::map<std::string, std::string> p;
  ASSERT_THROW(f.create("dynamic", p), converter_exception);

  p["path"] = "unknown_file_name";
  ASSERT_THROW(f.create("dynamic", p), converter_exception);

  p["path"] = LIBFILTER_SAMPLE;
  ASSERT_THROW(f.create("dynamic", p), converter_exception);

  p["function"] = "create";
  jubatus::util::lang::shared_ptr<string_filter> s(f.create("dynamic", p));

  std::string in("hoge-hoge");
  std::string out;
  s->filter(in, out);
  EXPECT_EQ("hoge hoge", out);
}

TEST(so_factory, num_filter_factory) {
  so_factory fa;
  num_filter_factory f(
        bind(&so_factory::create_num_filter, &fa, _1, _2));
  std::map<std::string, std::string> params;
  EXPECT_THROW(f.create("dynamic", params), converter_exception);

  params["path"] = "unknown_file_name";
  EXPECT_THROW(f.create("dynamic", params), converter_exception);

  params["path"] = LIBNUM_FILTER_SAMPLE;
  EXPECT_THROW(f.create("dynamic", params), converter_exception);

  params["function"] = "unknown_func";
  EXPECT_THROW(f.create("dynamic", params), converter_exception);

  params["function"] = "create";
  jubatus::util::lang::shared_ptr<num_filter> s(f.create("dynamic", params));

  EXPECT_EQ(6.0, s->filter(3.0));
}

}  // namespace fv_converter
}  // namespace server
}  // namespace jubatus

