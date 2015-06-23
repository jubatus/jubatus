// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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
#include <jubatus/client/stat_client.hpp>
#include "util.hpp"
#include "status_test.hpp"

typedef jubatus::stat::client::stat stat_client;

TEST(stat_test, get_config) {
  stat_client cli(host(), port(), cluster_name(), timeout());
  ASSERT_NE("", cli.get_config());
}

TEST(stat_test, get_status) {
  stat_client cli(host(), port(), cluster_name(), timeout());

  std::map<std::string, std::map<std::string, std::string> >
    res = cli.get_status();

  ASSERT_NE(0, res.size());

  std::map<std::string, std::map<std::string, std::string> >
      ::iterator it = res.begin();
  while(it != res.end()) {
    ASSERT_NE(0, it->second.size());
    ASSERT_NO_FATAL_FAILURE(assert_common_status(it->second));
    ASSERT_TRUE(has_key(it->second, "type"));
    ASSERT_EQ("stat", it->second["type"]);
    ++it;
  }
}

TEST(stat_test, save_load) {
  stat_client cli(host(), port(), cluster_name(), timeout());
  ASSERT_EQ(1, cli.save("stat.save_test.model").size());
  ASSERT_TRUE(cli.load("stat.save_test.model"));
}

TEST(stat_test, push) {
  stat_client cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.push("key", 0.0));
}

TEST(stat_test, sum) {
  stat_client cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.push("key", 0.0));
  cli.sum("key");
}


TEST(stat_test, stddev) {
  stat_client cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.push("key", 0.0));
  cli.stddev("key");
}

TEST(stat_test, max) {
  stat_client cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.push("key", 0.0));
  cli.max("key");
}

TEST(stat_test, min) {
  stat_client cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.push("key", 0.0));
  cli.min("key");
}

TEST(stat_test, entropy) {
  stat_client cli(host(), port(), cluster_name(), timeout());
  cli.entropy("key");
}

TEST(stat_test, moment) {
  stat_client cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.push("key", 0.0));
  cli.moment("key", 1, 1.0);
}

TEST(stat_test, clear) {
  stat_client cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.clear());
}
