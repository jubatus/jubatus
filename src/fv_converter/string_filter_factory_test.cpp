// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <string>
#include <pficommon/lang/scoped_ptr.h>
#include "string_filter_factory.hpp"
#include "exception.hpp"

#ifdef HAVE_RE2
#include "re2_filter.hpp"
#endif

using namespace jubatus;
using namespace jubatus::fv_converter;
using namespace std;
using namespace pfi::lang;

#ifdef HAVE_RE2
TEST(string_filter_factory, trivial) {
  string_filter_factory f;
  map<string, string> p;
  p["pattern"] = ".*";
  p["replace"] = "";

  scoped_ptr<string_filter> filter(f.create("regexp", p));
  EXPECT_TRUE(typeid(*filter) == typeid(re2_filter));
}
#endif

TEST(string_filter_factory, unknown) {
  string_filter_factory f;
  map<string, string> p;
  EXPECT_THROW(f.create("unknown", p), converter_exception);
}

TEST(string_filter_factory, dynamic) {
  string_filter_factory f;
  map<string, string> p;
  ASSERT_THROW(f.create("dynamic", p), converter_exception);

  p["path"] = "unknown_file_name";
  ASSERT_THROW(f.create("dynamic", p), converter_exception);

  p["path"] = LIBFILTER_SAMPLE;
  ASSERT_THROW(f.create("dynamic", p), converter_exception);

  p["function"] = "create";
  scoped_ptr<string_filter> s(f.create("dynamic", p));

  string in("hoge-hoge");
  string out;
  s->filter(in, out);
  EXPECT_EQ("hoge hoge", out);
}
