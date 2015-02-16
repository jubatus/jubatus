// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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
#include "dynamic_string_feature.hpp"
#include "jubatus/core/fv_converter/exception.hpp"

using jubatus::core::fv_converter::converter_exception;

namespace jubatus {
namespace server {
namespace fv_converter {

TEST(dynamic_string_feature, trivial) {
  dynamic_string_feature s(LIBSPLITTER_SAMPLE,
      "create",
      std::map<std::string, std::string>());
  std::vector<core::fv_converter::string_feature_element> elements;
  s.extract(" test test", elements);

  ASSERT_EQ(2u, elements.size());
  EXPECT_EQ(1u, elements[0].begin);
  EXPECT_EQ(4u, elements[0].length);
  EXPECT_EQ("test", elements[0].value);
  EXPECT_EQ(1.0, elements[0].score);
  EXPECT_EQ(6u, elements[1].begin);
  EXPECT_EQ(4u, elements[1].length);
  EXPECT_EQ("test", elements[1].value);
  EXPECT_EQ(1.0, elements[1].score);
}

TEST(dynamic_string_feature, unknown_file) {
  EXPECT_THROW(
      dynamic_string_feature s("unknown_file.so",
          "create",
          std::map<std::string, std::string>()),
      converter_exception);
}

TEST(dynamic_string_feature, unknown_function) {
  EXPECT_THROW(
      dynamic_string_feature s(LIBSPLITTER_SAMPLE,
          "unknown_function",
          std::map<std::string, std::string>()),
      converter_exception);
}

}  // namespace fv_converter
}  // namespace server
}  // namespace jubatus
