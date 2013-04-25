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
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include <pficommon/lang/scoped_ptr.h>
#include "dynamic_splitter.hpp"
#include "exception.hpp"
#include "word_splitter.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(dynamic_splitter, trivial) {
  dynamic_splitter s(LIBSPLITTER_SAMPLE,
      "create",
      std::map<std::string, std::string>());
  std::vector<std::pair<size_t, size_t> > bounds;
  s.split(" test test", bounds);

  ASSERT_EQ(2u, bounds.size());
  EXPECT_EQ(1u, bounds[0].first);
  EXPECT_EQ(4u, bounds[0].second);
  EXPECT_EQ(6u, bounds[1].first);
  EXPECT_EQ(4u, bounds[1].second);
}

TEST(dynamic_splitter, unknown_file) {
  EXPECT_THROW(
      dynamic_splitter s("unknown_file.so",
          "create",
          std::map<std::string, std::string>()),
      converter_exception);
}

TEST(dynamic_splitter, unknown_function) {
  EXPECT_THROW(
      dynamic_splitter s(LIBSPLITTER_SAMPLE,
          "unknown_function",
          std::map<std::string, std::string>()),
      converter_exception);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
