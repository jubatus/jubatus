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

#include "filesystem.hpp"
#include <limits.h>
#include <string>
#include <cstdlib>
#include <gtest/gtest.h>

TEST(filesystem, base_name) {
  EXPECT_EQ("test", jubatus::server::common::base_name("/path/to/test"));
  EXPECT_EQ("basename", jubatus::server::common::base_name("basename"));
  EXPECT_EQ("", jubatus::server::common::base_name("/path/to/"));
}

TEST(filesystem, is_writable) {
  std::string path = "tmp_test_directory";
  mkdir(path.c_str(), S_IWUSR);
  EXPECT_EQ(true, jubatus::server::common::is_writable(path.c_str()));
  rmdir(path.c_str());
}

TEST(filesystem, real_path) {
  char buf[PATH_MAX];
  EXPECT_EQ(getcwd(buf, PATH_MAX), jubatus::server::common::real_path("."));
  EXPECT_EQ("/", jubatus::server::common::real_path("/"));
}
