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
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "space_splitter.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

namespace {

void test_split(
    const std::string& string,
    const std::vector<std::pair<size_t, size_t> >& expected) {
  space_splitter splitter;

  std::vector<std::pair<size_t, size_t> > actual;
  splitter.split(string, actual);

  ASSERT_EQ(expected, actual);
}

}  // namespace

TEST(space_splitter, empty) {
  std::vector<std::pair<size_t, size_t> > empty;
  test_split("", empty);
  test_split("   ", empty);
  test_split("\n\t", empty);
}

TEST(space_splitter, trivial) {
  std::vector<std::pair<size_t, size_t> > bounds;
  bounds.push_back(std::make_pair(0, 4));

  test_split("hoge", bounds);
  test_split("hoge ", bounds);
  test_split("hoge\n", bounds);
  test_split("hoge\t", bounds);

  bounds.clear();
  bounds.push_back(std::make_pair(1, 4));
  test_split(" hoge", bounds);
  test_split(" hoge ", bounds);
  test_split("\nhoge\n", bounds);
  test_split("\thoge\t", bounds);

  bounds.clear();
  bounds.push_back(std::make_pair(2, 4));
  test_split("  hoge", bounds);
  test_split("  hoge  ", bounds);

  bounds.clear();
  bounds.push_back(std::make_pair(2, 4));
  bounds.push_back(std::make_pair(8, 4));
  bounds.push_back(std::make_pair(14, 4));
  test_split("  hoge  hoge  hoge  ", bounds);

  bounds.clear();
  bounds.push_back(std::make_pair(2, 1));
  bounds.push_back(std::make_pair(5, 1));
  test_split("  .  .", bounds);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
