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

#include <gtest/gtest.h>
#include <string>
#include "config_util.hpp"

using std::string;
using pfi::text::json::is;
using pfi::text::json::json;
using pfi::text::json::json_integer;
using pfi::text::json::json_object;
using pfi::text::json::json_string;

namespace jubatus {

TEST(config_util, get_param_bad_type) {
  json js(new json_integer(0));
  EXPECT_EQ("abc", get_param(js, "test", string("abc")));
}

TEST(config_util, get_param_default) {
  json js(new json_object());
  js["nest"] = json(new json_string("nest_value"));

  // key not found
  EXPECT_EQ("abc", get_param(js, "not_found", string("abc")));
  // nest
  EXPECT_EQ("nest_value", get_param(js, "nest", string("value")));
}

TEST(config_util, get_param_obj_null) {
  json js(new json_object());
  js["nest"] = json(new json_object());

  EXPECT_TRUE(is<json_null>(get_param_obj(js, "not_found")));
  EXPECT_TRUE(is<json_object>(get_param_obj(js, "nest")));
}

TEST(config_util, json_object) {
  json js(new json_object());
  json js_nest(new json_object());
  js_nest["nest_key"] = json(new json_string("nest_value"));
  js["test"] = js_nest;

  json nest = get_param_obj(js, "test");

  ASSERT_TRUE(is<json_object>(nest));
  ASSERT_TRUE(is<json_string>(nest["nest_key"]));
  EXPECT_EQ("nest_value", json_cast<string>(nest["nest_key"]));
}

}  // namespace jubatus
