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
#include <string>

#include "counter.hpp"


using namespace std;
using namespace jubatus::fv_converter;

namespace jubatus {

TEST(counter, trivial) {
  counter<string> c;
  EXPECT_FALSE(c.contains("hoge"));
  
  EXPECT_EQ(0u, ((const counter<string>&)c)["hoge"]);
  EXPECT_EQ(0u, c["hoge"]);
  c["hoge"] = 1;
  c["fuga"] = 2;
  EXPECT_EQ(1u, c["hoge"]);
  EXPECT_EQ(2u, c["fuga"]);
}

TEST(counter, add) {
  counter<string> x, y;
  x["hoge"] = 1;
  x["fuga"] = 2;

  y["foo"] = 5;
  y["hoge"] = 3;

  x.add(y);

  EXPECT_EQ(4u, x["hoge"]);
  EXPECT_EQ(2u, x["fuga"]);
  EXPECT_EQ(5u, x["foo"]);
}

}
