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
#include <jubatus/client/regression_client.hpp>
#include "util.hpp"
#include "status_test.hpp"

using std::vector;
using jubatus::client::common::datum;
using jubatus::regression::client::regression;
using jubatus::regression::scored_datum;

namespace {
vector<scored_datum> make_scored_data() {
  vector<scored_datum> data;
  jubatus::client::common::datum dat;
  data.push_back(scored_datum(1.0, dat));
  return data;
}
vector<jubatus::client::common::datum> make_data() {
  vector<jubatus::client::common::datum> data;
  return data;
}
}

TEST(regression_test, get_config) {
  regression cli(host(), port(), cluster_name(), timeout());
  ASSERT_NE("", cli.get_config());
}

TEST(regression_test, get_status) {
  regression cli(host(), port(), cluster_name(), timeout());

  std::map<std::string, std::map<std::string, std::string> >
    res = cli.get_status();

  ASSERT_NE(0, res.size());

  std::map<std::string, std::map<std::string, std::string> >
      ::iterator it = res.begin();
  while(it != res.end()) {
    ASSERT_NE(0, it->second.size());
    ASSERT_NO_FATAL_FAILURE(assert_common_status(it->second));
    ASSERT_TRUE(has_key(it->second, "type"));
    ASSERT_EQ("regression", it->second["type"]);
    ++it;
  }
}

TEST(regression_test, save_load) {
  regression cli(host(), port(), cluster_name(), timeout());
  ASSERT_EQ(1, cli.save("regression.save_test.model").size());
  ASSERT_TRUE(cli.load("regression.save_test.model"));
}

TEST(regression_test, train) {
  regression cli(host(), port(), cluster_name(), timeout());
  cli.train(make_scored_data());
}

TEST(regression_test, estimate) {
  regression cli(host(), port(), cluster_name(), timeout());
  vector<float> result = cli.estimate(vector<datum>());
  result.size();
}

TEST(regression_test, clear) {
  regression cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.clear());
}
