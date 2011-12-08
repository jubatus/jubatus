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
#include "membership.hpp"

using namespace std;

namespace jubatus {
namespace common {

TEST(util, build_loc_str) {
  EXPECT_EQ("127.0.0.1_9199", build_loc_str("127.0.0.1", 9199));
}

TEST(util, build_existence_path) {
  string s;
  EXPECT_TRUE(build_existence_path("/path/base", "127.0.0.1", 9199, s));
  EXPECT_EQ("/path/base/127.0.0.1_9199", s);
}

TEST(util, revert) {
  string name = "127.0.0.1_9199";
  string ip;
  int port;
  ASSERT_TRUE(revert(name, ip, port));
  EXPECT_EQ("127.0.0.1", ip);
  EXPECT_EQ(9199, port);
}

}
}
