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

#include "util.hpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include <iostream>

TEST(common,util){
  const std::string env = getenv("PATH");
  jubatus::util::append_env_path("PATH", "./path/to/hoge");
  const std::string env2 = getenv("PATH");
  ASSERT_TRUE(env.size() < env2.size());
  //  std::cout << env << std::endl;
  //  std::cout << env2 << std::endl;
  ASSERT_NE(env, env2);
}

TEST(common,util2){
  const std::string env = getenv("PATH");
  jubatus::util::append_server_path("./path/to/hoge");
  const std::string env2 = getenv("PATH");
  ASSERT_TRUE(env.size() < env2.size());
  //  std::cout << env << std::endl;
  //  std::cout << env2 << std::endl;
  ASSERT_NE(env, env2);
}
