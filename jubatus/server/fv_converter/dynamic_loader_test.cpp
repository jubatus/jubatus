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

#include <stdlib.h>
#include <map>
#include <string>
#include <gtest/gtest.h>
#include "dynamic_loader.hpp"
#include "jubatus/core/fv_converter/exception.hpp"
#include "jubatus/core/fv_converter/word_splitter.hpp"

using jubatus::core::fv_converter::converter_exception;
using jubatus::core::fv_converter::word_splitter;

namespace jubatus {
namespace server {
namespace fv_converter {

TEST(dynamic_loader, trivial) {
  dynamic_loader l(LIBSPLITTER_SAMPLE);
  l.load_symbol("create");
  {
    dynamic_loader l2(LIBSPLITTER_SAMPLE);
    // destroctor of l2 call dlclose, but you can use l
  }
  l.load_symbol("create");
}

TEST(dynamic_loader, unknown_so_file) {
  EXPECT_THROW(dynamic_loader lo("unknown_file.so"), converter_exception);
}

TEST(dynamic_loader, unknown_function) {
  dynamic_loader l(LIBSPLITTER_SAMPLE);
  EXPECT_THROW(l.load_symbol("unknown"), converter_exception);
}

TEST(dynamic_loader, null) {
  dynamic_loader l(LIBSPLITTER_NULL);
  EXPECT_THROW(load_object<word_splitter>(
      l, std::string("create"), std::map<std::string, std::string>()),
               converter_exception);
}

TEST(dynamic_loader, absolute_path) {
  ASSERT_EQ('/', BUILD_DIR[0]);
  const char* absolute_path
      = BUILD_DIR "/jubatus/server/fv_converter/" LIBSPLITTER_SAMPLE;
  dynamic_loader l(absolute_path);
  l.load_symbol("create");
}

TEST(dynamic_loader, load_from_env) {
  ASSERT_EQ('/', BUILD_DIR[0]);
  const char* plugin_dir = BUILD_DIR "/jubatus/server/fv_converter";
  ASSERT_EQ(0, ::setenv("JUBATUS_PLUGIN_PATH", plugin_dir, 1));
  dynamic_loader l(LIBSPLITTER_SAMPLE);
  l.load_symbol("create");
}

}  // namespace fv_converter
}  // namespace server
}  // namespace jubatus
