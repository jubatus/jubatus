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

#include <string>
#include <gtest/gtest.h>
#include "regexp_filter.hpp"
#include "exception.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(regexp_filter, trivial) {
  regexp_filter f("a+", "AA");

  std::string out;
  f.filter("auauaa", out);
  EXPECT_EQ("AAuAAuAA", out);

  f.filter("auauaab", out);
  EXPECT_EQ("AAuAAuAAb", out);
}

TEST(regexp_filter, bos) {
  regexp_filter f("^", "A");

  std::string out;
  f.filter("BCD", out);
  EXPECT_EQ("ABCD", out);

  f.filter("", out);
  EXPECT_EQ("A", out);
}

TEST(regexp_filter, eos) {
  regexp_filter f("$", "A");

  std::string out;
  f.filter("BCD", out);
  EXPECT_EQ("BCDA", out);

  f.filter("", out);
  EXPECT_EQ("A", out);
}

TEST(regexp_filter, non_greedy) {
  regexp_filter f(".??", "A");

  std::string out;
  f.filter("BCD", out);
  EXPECT_EQ("ABACADA", out);
  f.filter("", out);
  EXPECT_EQ("A", out);
}

TEST(regexp_filter, illegal) {
  ASSERT_THROW(regexp_filter f("*hoge", "replace"), converter_exception);
}

TEST(regexp_filter, detag) {
  regexp_filter f("<.*?>", "");

  std::string out;
  f.filter("<dlsfjasfasfdsaf>", out);
  EXPECT_EQ("", out);
  f.filter("<>foo bar<>", out);
  EXPECT_EQ("foo bar", out);
  f.filter("<html>foo bar</html>", out);
  EXPECT_EQ("foo bar", out);
}

TEST(regexp_filter, substitute) {
  regexp_filter f("AA", "AAAA");

  std::string out;
  f.filter("AA", out);
  EXPECT_EQ("AAAA", out);
  f.filter("AAAAAAA", out);
  EXPECT_EQ("AAAAAAAAAAAAA", out);
}

TEST(regexp_filter, long_substitute) {
  for (int i = 0; i < 500; i += 3) {
    std::string long_string = "a";
    for (int j = 0; j < i; ++j) {
      long_string += "a";
    }
    regexp_filter f("<.*?>", long_string);

    std::string out;
    f.filter("<dlsfjasfasfdsaf>", out);
    EXPECT_EQ(long_string, out);
    f.filter("<>foo bar<>", out);
    EXPECT_EQ(long_string + "foo bar" + long_string, out);
    f.filter("<html>foo bar</html>", out);
    EXPECT_EQ(long_string + "foo bar" + long_string, out);
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
