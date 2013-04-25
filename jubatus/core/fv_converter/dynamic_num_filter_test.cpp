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

#include <map>
#include <string>
#include <gtest/gtest.h>
#include "dynamic_num_filter.hpp"
#include "exception.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(dynamic_num_filter, trivial) {
  std::map<std::string, std::string> params;

  dynamic_num_filter f(LIBNUM_FILTER_SAMPLE,
      "create",
      params);
  EXPECT_EQ(6.0, f.filter(3.0));
}

TEST(dynamic_num_filter, unknown_file) {
  std::map<std::string, std::string> params;
  EXPECT_THROW(dynamic_num_filter f("unkonwn_file.so",
          "create",
          params),
      converter_exception);
}

TEST(dynamic_num_filter, unknown_function) {
  std::map<std::string, std::string> params;
  EXPECT_THROW(
      dynamic_num_filter f(LIBNUM_FILTER_SAMPLE, "unknown_function", params),
      converter_exception);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
