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
#include <jubatus/client/nearest_neighbor_client.hpp>
#include "util.hpp"
#include "status_test.hpp"

using std::vector;
using jubatus::client::common::datum;
using jubatus::nearest_neighbor::client::nearest_neighbor;
using jubatus::nearest_neighbor::id_with_score;

TEST(nearest_neighbor_test, get_config) {
  nearest_neighbor cli(host(), port(), cluster_name(), timeout());
  ASSERT_NE("", cli.get_config());
}

TEST(nearest_neighbor_test, get_status) {
  nearest_neighbor cli(host(), port(), cluster_name(), timeout());

  std::map<std::string, std::map<std::string, std::string> >
    res = cli.get_status();

  ASSERT_NE(0, res.size());

  std::map<std::string, std::map<std::string, std::string> >
      ::iterator it = res.begin();
  while(it != res.end()) {
    ASSERT_NE(0, it->second.size());
    ASSERT_NO_FATAL_FAILURE(assert_common_status(it->second));
    ASSERT_TRUE(has_key(it->second, "type"));
    ASSERT_EQ("nearest_neighbor", it->second["type"]);
    ++it;
  }
}

TEST(nearest_neighbor_test, save_load) {
  nearest_neighbor cli(host(), port(), cluster_name(), timeout());
  ASSERT_EQ(1, cli.save("nearest_neighbor.save_test.model").size());
  ASSERT_TRUE(cli.load("nearest_neighbor.save_test.model"));
}

TEST(nearest_neighbor_test, set_row) {
  nearest_neighbor cli(host(), port(), cluster_name(), timeout());
  datum d;
  d.add_number("key", 1.0);
  ASSERT_TRUE(cli.set_row("id", d));
}

TEST(nearest_neighbor_test, neighbor_row_from_id) {
  nearest_neighbor cli(host(), port(), cluster_name(), timeout());
  datum d;
  d.add_number("key", 1.0);
  ASSERT_TRUE(cli.set_row("neighbor_row_from_id", d));
  cli.neighbor_row_from_id("neighbor_row_from_id", 1);
}

TEST(nearest_neighbor_test, neighbor_row_from_datum) {
  nearest_neighbor cli(host(), port(), cluster_name(), timeout());
  datum d;
  d.add_number("neighbor_row_from_datum", 1.0);
  ASSERT_TRUE(cli.set_row("neighbor_row_from_datum", d));
  cli.neighbor_row_from_datum(d, 1);
}

TEST(nearest_neighbor_test, similar_row_from_id) {
  nearest_neighbor cli(host(), port(), cluster_name(), timeout());
  datum d;
  d.add_number("neighbor_row_from_datum", 1.0);
  ASSERT_TRUE(cli.set_row("similar_row_from_id", d));
  cli.similar_row_from_id("similar_row_from_id", 1);
}

TEST(nearest_neighbor_test, similar_row_from_datum) {
  nearest_neighbor cli(host(), port(), cluster_name(), timeout());
  datum d;
  d.add_number("neighbor_row_from_datum", 1.0);
  cli.similar_row_from_datum(d, 1);
}

TEST(nearest_neighbor_test, clear) {
  nearest_neighbor cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.clear());
}


