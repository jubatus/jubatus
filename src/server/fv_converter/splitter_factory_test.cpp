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

#include <map>
#include <string>
#include <pficommon/lang/scoped_ptr.h>

#include "splitter_factory.hpp"
#include "word_splitter.hpp"
#include "exception.hpp"

using namespace std;
using namespace jubatus;
using namespace pfi::lang;

TEST(splitter_factory, trivial) {
  splitter_factory f;
  map<string, string> param;
  ASSERT_THROW(f.create("hoge", param), converter_exception);
}

TEST(splitter_factory, dynamic) {
  splitter_factory f;
  map<string, string> param;
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["path"] = "unknown_file_name";
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["path"] = "./libsplitter_sample.so";
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["function"] = "create";
  scoped_ptr<word_splitter> s(f.create("dynamic", param));

  string d("hoge fuga");
  vector<pair<size_t, size_t> > bs;
  s->split(d, bs);
  ASSERT_EQ(2u, bs.size());
  ASSERT_EQ(0u, bs[0].first);
  ASSERT_EQ(4u, bs[0].second);
  ASSERT_EQ(5u, bs[1].first);
  ASSERT_EQ(4u, bs[1].second);
}

TEST(splitter_factory, ngram) {
  splitter_factory f;
  map<string, string> param;
  ASSERT_THROW(f.create("ngram", param), converter_exception);

  param["char_num"] = "not_a_number";
  ASSERT_THROW(f.create("ngram", param), converter_exception);

  param["char_num"] = "-1";
  ASSERT_THROW(f.create("ngram", param), converter_exception);

  param["char_num"] = "2";
  scoped_ptr<word_splitter> s(f.create("ngram", param));
}
