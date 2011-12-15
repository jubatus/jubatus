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
#include <vector>
#include <string>
#include <map>

#include <pficommon/lang/scoped_ptr.h>

#include "mecab_splitter.hpp"
#include "../../fv_converter/test_util.hpp"
#include "../../fv_converter/exception.hpp"

using namespace std;

namespace jubatus {

using fv_converter::word_splitter;
using fv_converter::converter_exception;

TEST(mecab_splitter, trivial) {
  mecab_splitter m;
  vector<pair<size_t, size_t> > bs;
  m.split("本日は晴天なり", bs);
  vector<pair<size_t, size_t> > exp;
  
  exp.push_back(make_pair(0, 6));
  exp.push_back(make_pair(6, 3));
  exp.push_back(make_pair(9, 6));
  exp.push_back(make_pair(15, 6));

  PairVectorEquals(exp, bs);
}

TEST(mecab_splitter, illegal_argument) {
  EXPECT_THROW(mecab_splitter("-h"), converter_exception);
  EXPECT_THROW(mecab_splitter("-v"), converter_exception);
}

TEST(mecab_splitter_create, trivial) {
  map<string, string> param;
  pfi::lang::scoped_ptr<word_splitter> s(create(param));
  string d("東京へ行く");
  vector<pair<size_t, size_t> > bs;
  s->split(d, bs);
  ASSERT_EQ(3u, bs.size());
  ASSERT_EQ(0u, bs[0].first);
  ASSERT_EQ(6u, bs[0].second);
  ASSERT_EQ(6u, bs[1].first);
  ASSERT_EQ(3u, bs[1].second);
  ASSERT_EQ(9u, bs[2].first);
  ASSERT_EQ(6u, bs[2].second);
}

TEST(mecab_splitter_create, illegal_argument) {
  map<string, string> param;
  param["arg"] = "-h";
  EXPECT_THROW(pfi::lang::scoped_ptr<word_splitter>(create(param)), 
               converter_exception);
}

}
