// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
#include <pficommon/lang/scoped_ptr.h>
#include "word_splitter.hpp"
#include "exception.hpp"
#include "dynamic_splitter.hpp"

using namespace std;
using namespace jubatus;
using namespace jubatus::fv_converter;
using namespace pfi::lang;

TEST(dynamic_splitter, trivial) {
  dynamic_splitter s(LIBSPLITTER_SAMPLE,
                     "create",
                     map<string, string>());
    vector<pair<size_t, size_t> > bounds;
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
                         map<string, string>()),
      converter_exception);
}

TEST(dynamic_splitter, unknown_function) {
  EXPECT_THROW(
      dynamic_splitter s(LIBSPLITTER_SAMPLE,
                         "unknown_function",
                         map<string, string>()),
      converter_exception);
}
