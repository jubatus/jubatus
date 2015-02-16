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
#include <jubatus/client/anomaly_client.hpp>
#include "util.hpp"
#include "status_test.hpp"

using std::string;
using std::vector;
using jubatus::client::common::datum;
using jubatus::anomaly::client::anomaly;
using jubatus::anomaly::id_with_score;

TEST(anomaly_test, get_config) {
  anomaly cli(host(), port(), cluster_name(), timeout());
  ASSERT_NE("", cli.get_config());
}

TEST(anomaly_test, get_status) {
  anomaly cli(host(), port(), cluster_name(), timeout());

  std::map<std::string, std::map<std::string, std::string> >
    res = cli.get_status();

  ASSERT_NE(0, res.size());

  std::map<std::string, std::map<std::string, std::string> >
      ::iterator it = res.begin();
  while(it != res.end()) {
    ASSERT_NE(0, it->second.size());
    ASSERT_NO_FATAL_FAILURE(assert_common_status(it->second));
    ASSERT_TRUE(has_key(it->second, "type"));
    ASSERT_EQ("anomaly", it->second["type"]);
    ++it;
  }
}

TEST(anomaly_test, save_load) {
  anomaly cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.save("anomaly.save_test.model"));
  ASSERT_TRUE(cli.load("anomaly.save_test.model"));
}

TEST(anomaly_test, clear_row) {
  anomaly cli(host(), port(), cluster_name(), timeout());
  datum d;
  id_with_score id = cli.add(d);
  try {
    cli.clear_row(id.id);
  } catch (...) {
    // some algorithm doesn't support this method
  }
}

TEST(anomaly_test, add) {
  anomaly cli(host(), port(), cluster_name(), timeout());
  datum d;
  id_with_score id = cli.add(d);
}

TEST(anomaly_test, update) {
  anomaly cli(host(), port(), cluster_name(), timeout());
  datum d;
  id_with_score id = cli.add(d);
  float score = cli.update(id.id, d);
}

TEST(anomaly_test, overwrite) {
  anomaly cli(host(), port(), cluster_name(), timeout());
  datum d;
  id_with_score id = cli.add(d);
  try {
    float score = cli.overwrite(id.id, d);
  } catch (...) {
    // some algorithm doesn't support this method
  }
}

TEST(anomaly_test, calc_score) {
  anomaly cli(host(), port(), cluster_name(), timeout());
  datum d;
  id_with_score id = cli.add(d);
  float score = cli.calc_score(d);
}

TEST(anomaly_test, get_all_rows) {
  anomaly cli(host(), port(), cluster_name(), timeout());
  vector<string> rows = cli.get_all_rows();
}

TEST(anomaly_test, clear) {
  anomaly cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.clear());
}
