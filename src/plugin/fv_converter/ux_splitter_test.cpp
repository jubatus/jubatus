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
#include "ux_splitter.hpp"
#include "../../fv_converter/exception.hpp"

using namespace std;

namespace jubatus {

using fv_converter::word_splitter;
using fv_converter::converter_exception;

TEST(ux_splitter, split) {
  vector<string> ks;
  const char* keywords[] = {
    "ueno",
    "tokyo",
    "shinjuku",
    "ikebukuro"
  };
  string doc = "ueno tokyo shinjuku ikebukuro";

  for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
    ks.push_back(keywords[i]);
  }
    vector<pair<size_t, size_t> > bounds;
  {
    ux_splitter splitter(ks);
    splitter.split(doc, bounds);
    ASSERT_EQ(4u, bounds.size());
  }
  {
    vector<string> ks2;
    ks2.push_back("o");
    ux_splitter splitter(ks2);

    splitter.split(doc, bounds);
    ASSERT_EQ(4u, bounds.size());
  }
}

TEST(ux_splitter, create) {
  map<string, string> param;
  ASSERT_THROW(create(param), converter_exception);

  param["path"] = "unknown_file_name";
  ASSERT_THROW(create(param), converter_exception);

  param["dict_path"] = "../../server/fv_converter/test_input/keywords";
  pfi::lang::scoped_ptr<word_splitter> s(create(param));

  string d("hoge fuga");
  vector<pair<size_t, size_t> > bs;
  s->split(d, bs);
  ASSERT_EQ(2u, bs.size());
  ASSERT_EQ(0u, bs[0].first);
  ASSERT_EQ(4u, bs[0].second);
  ASSERT_EQ(5u, bs[1].first);
  ASSERT_EQ(4u, bs[1].second);
}

}
