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
#include <jubatus/client/clustering_client.hpp>
#include "util.hpp"
#include "status_test.hpp"

using std::vector;
using jubatus::client::common::datum;
using jubatus::clustering::client::clustering;
using jubatus::clustering::weighted_datum;

TEST(clustering_test, get_config) {
  clustering cli(host(), port(), cluster_name(), timeout());
  ASSERT_NE("", cli.get_config());
}

TEST(clustering_test, get_status) {
  clustering cli(host(), port(), cluster_name(), timeout());

  std::map<std::string, std::map<std::string, std::string> >
    res = cli.get_status();

  ASSERT_NE(0, res.size());

  std::map<std::string, std::map<std::string, std::string> >
      ::iterator it = res.begin();
  while(it != res.end()) {
    ASSERT_NE(0, it->second.size());
    ASSERT_NO_FATAL_FAILURE(assert_common_status(it->second));
    ASSERT_TRUE(has_key(it->second, "type"));
    ASSERT_EQ("clustering", it->second["type"]);
    ++it;
  }
}

TEST(clustering_test, save_load) {
  clustering cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.save("clustering.save_test.model"));
  ASSERT_TRUE(cli.load("clustering.save_test.model"));
}

TEST(clustering_test, push) {
  clustering cli(host(), port(), cluster_name(), timeout());
  datum d;
  vector<datum> points(1, d);
  ASSERT_TRUE(cli.push(points));
}

TEST(clustering_test, get_revision) {
  clustering cli(host(), port(), cluster_name(), timeout());
  cli.get_revision();
}

TEST(clustering_test, get_core_members) {
  clustering cli(host(), port(), cluster_name(), timeout());
  vector<vector<weighted_datum> > result = cli.get_core_members();
}

TEST(clustering_test, get_k_center) {
  clustering cli(host(), port(), cluster_name(), timeout());
  vector<datum> result = cli.get_k_center();
}

TEST(clustering_test, get_nearest_center) {
  clustering cli(host(), port(), cluster_name(), timeout());
  datum d;
  cli.get_nearest_center(d);
}

TEST(clustering_test, get_nearest_members) {
  clustering cli(host(), port(), cluster_name(), timeout());
  datum d;
  vector<weighted_datum> result = cli.get_nearest_members(d);
}

// There is no `clear` method in clustering.

