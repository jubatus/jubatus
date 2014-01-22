// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <jubatus/client/recommender_client.hpp>
#include "util.hpp"
#include "status_test.hpp"

using std::string;
using std::vector;
using jubatus::client::common::datum;
using jubatus::recommender::client::recommender;
using jubatus::recommender::id_with_score;

TEST(recommender_test, get_config) {
  recommender cli(host(), port(), cluster_name(), timeout());
  ASSERT_NE("", cli.get_config());
}

TEST(recommender_test, get_status) {
  recommender cli(host(), port(), cluster_name(), timeout());

  std::map<std::string, std::map<std::string, std::string> >
    res = cli.get_status();

  ASSERT_NE(0, res.size());

  std::map<std::string, std::map<std::string, std::string> >
      ::iterator it = res.begin();
  while(it != res.end()) {
    ASSERT_NE(0, it->second.size());
    ASSERT_NO_FATAL_FAILURE(assert_common_status(it->second));
    ASSERT_TRUE(has_key(it->second, "type"));
    ASSERT_EQ("recommender", it->second["type"]);
    ++it;
  }
}

TEST(recommender_test, save_load) {
  recommender cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.save("recommender.save_test.model"));
  ASSERT_TRUE(cli.load("recommender.save_test.model"));
}

TEST(recommender_test, clear_row) {
  recommender cli(host(), port(), cluster_name(), timeout());
  cli.clear_row("clear_row_id");
}

TEST(recommender_test, update_row) {
  recommender cli(host(), port(), cluster_name(), timeout());
  datum dat;
  dat.add_number("key", 1.0);
  cli.update_row("update_row_id", dat);
}

TEST(recommender_test, complete_row_from_id) {
  recommender cli(host(), port(), cluster_name(), timeout());
  datum dat;
  dat.add_number("key", 1.0);
  ASSERT_TRUE(cli.update_row("complete_row_from_id", dat));
  cli.complete_row_from_id("complete_row_from_id");
}

TEST(recommender_test, complete_row_from_datum) {
  recommender cli(host(), port(), cluster_name(), timeout());
  datum dat;
  dat.add_number("key", 1.0);
  ASSERT_TRUE(cli.update_row("complete_row_from_datum", dat));
  datum res = cli.complete_row_from_datum(dat);
}

TEST(recommender_test, similar_row_from_id) {
  recommender cli(host(), port(), cluster_name(), timeout());
  datum dat;
  dat.add_number("key", 1.0);
  ASSERT_TRUE(cli.update_row("similar_row_from_id", dat));
  vector<id_with_score> res = cli.similar_row_from_id("similar_row_from_id", 1);
}

TEST(recommender_test, similar_row_from_datum) {
  recommender cli(host(), port(), cluster_name(), timeout());
  datum dat;
  dat.add_number("key", 1.0);
  ASSERT_TRUE(cli.update_row("similar_row_from_id", dat));
  vector<id_with_score> res = cli.similar_row_from_datum(dat, 1);
}

TEST(recommender_test, decode_row) {
  recommender cli(host(), port(), cluster_name(), timeout());
  datum dat;
  dat.add_number("key", 1.0);
  ASSERT_TRUE(cli.update_row("decode_row", dat));
  datum res = cli.decode_row("decode_row");
}

TEST(recommender_test, get_all_rows) {
  recommender cli(host(), port(), cluster_name(), timeout());
  vector<string> res = cli.get_all_rows();
  ASSERT_EQ(0, res.size());
}

TEST(recommender_test, calc_similarity) {
  recommender cli(host(), port(), cluster_name(), timeout());
  datum dat;
  dat.add_number("key", 1.0);
  cli.calc_similarity(dat, dat);
}

TEST(recommender_test, calc_l2norm) {
  recommender cli(host(), port(), cluster_name(), timeout());
  datum dat;
  dat.add_number("key", 1.0);
  cli.calc_l2norm(dat);
}

TEST(recommender_test, clear) {
  recommender cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.clear());
}
