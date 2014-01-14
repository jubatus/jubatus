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

#include "system.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <gtest/gtest.h>

TEST(system, env_path) {
  const std::string env = getenv("PATH");
  jubatus::server::common::append_env_path("PATH", "./path/to/hoge");
  const std::string env2 = getenv("PATH");
  ASSERT_TRUE(env.size() < env2.size());
  //  std::cout << env << std::endl;
  //  std::cout << env2 << std::endl;
  ASSERT_NE(env, env2);
}

TEST(system, append_server_path) {
  const std::string env = getenv("PATH");
  jubatus::server::common::append_server_path("./path/to/hoge");
  const std::string env2 = getenv("PATH");
  ASSERT_TRUE(env.size() < env2.size());
  //  std::cout << env << std::endl;
  //  std::cout << env2 << std::endl;
  ASSERT_NE(env, env2);
}

TEST(system, get_program_name) {
  std::string path;
  EXPECT_NO_THROW({
    path = jubatus::server::common::get_program_name();
  });
  EXPECT_EQ(std::string("system_test"), path);
}

TEST(system, get_user_name) {
  std::string user;
  EXPECT_NO_THROW({
    user = jubatus::server::common::get_user_name();
  });
  EXPECT_NE(std::string(""), user);
}

TEST(system, get_machine_status) {
  jubatus::server::common::machine_status_t status;
  EXPECT_NO_THROW({
    jubatus::server::common::get_machine_status(status);
  });
}
