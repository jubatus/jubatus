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

#include <gtest/gtest.h>

#include "test_util.hpp"
#include "space_splitter.hpp"

using namespace jubatus;
using namespace jubatus::fv_converter;
using namespace std;

static void TestSplit(const string& string,
                      const vector<pair<size_t, size_t> >& expected) {
  space_splitter splitter;

  vector<pair<size_t, size_t> > actual;
  splitter.split(string, actual);

  PairVectorEquals(expected, actual);
}

TEST(space_splitter, empty) {
  vector<pair<size_t, size_t> > empty;
  TestSplit("", empty);
  TestSplit("   ", empty);
  TestSplit("\n\t", empty);
}

TEST(space_splitter, trivial) {
  vector<pair<size_t, size_t> > bounds;
  bounds.push_back(make_pair(0, 4));

  TestSplit("hoge", bounds);
  TestSplit("hoge ", bounds);
  TestSplit("hoge\n", bounds);
  TestSplit("hoge\t", bounds);

  bounds.clear();
  bounds.push_back(make_pair(1, 4));
  TestSplit(" hoge", bounds);
  TestSplit(" hoge ", bounds);
  TestSplit("\nhoge\n", bounds);
  TestSplit("\thoge\t", bounds);

  bounds.clear();
  bounds.push_back(make_pair(2, 4));
  TestSplit("  hoge", bounds);
  TestSplit("  hoge  ", bounds);

  bounds.clear();
  bounds.push_back(make_pair(2, 4));
  bounds.push_back(make_pair(8, 4));
  bounds.push_back(make_pair(14, 4));
  TestSplit("  hoge  hoge  hoge  ", bounds);

  bounds.clear();
  bounds.push_back(make_pair(2, 1));
  bounds.push_back(make_pair(5, 1));
  TestSplit("  .  .", bounds);
}
