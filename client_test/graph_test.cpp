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
#include <jubatus/client/graph_client.hpp>
#include "util.hpp"
#include "status_test.hpp"

using std::string;
using std::map;
using jubatus::client::common::datum;
using jubatus::graph::client::graph;
using jubatus::graph::node;
using jubatus::graph::query;
using jubatus::graph::preset_query;
using jubatus::graph::edge;
using jubatus::graph::shortest_path_query;

TEST(graph_test, get_config) {
  graph cli(host(), port(), cluster_name(), timeout());
  ASSERT_NE("", cli.get_config());
}

TEST(graph_test, get_status) {
  graph cli(host(), port(), cluster_name(), timeout());

  std::map<std::string, std::map<std::string, std::string> >
    res = cli.get_status();

  ASSERT_NE(0, res.size());

  std::map<std::string, std::map<std::string, std::string> >
      ::iterator it = res.begin();
  while(it != res.end()) {
    ASSERT_NE(0, it->second.size());
    ASSERT_NO_FATAL_FAILURE(assert_common_status(it->second));
    ASSERT_TRUE(has_key(it->second, "type"));
    ASSERT_EQ("graph", it->second["type"]);
    ++it;
  }
}

TEST(graph_test, save_load) {
  graph cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.save("graph.save_test.model"));
  ASSERT_TRUE(cli.load("graph.save_test.model"));
}

TEST(graph_test, create_node) {
  graph cli(host(), port(), cluster_name(), timeout());
  cli.create_node();
}

TEST(graph_test, remove_node) {
  graph cli(host(), port(), cluster_name(), timeout());
  string id = cli.create_node();
  cli.remove_node(id);
}

TEST(graph_test, update_node) {
  graph cli(host(), port(), cluster_name(), timeout());
  string id = cli.create_node();
  map<string, string> property;
  cli.update_node(id, property);
}

TEST(graph_test, create_update_remove_edge) {
  graph cli(host(), port(), cluster_name(), timeout());
  string src = cli.create_node(), target = cli.create_node();
  map<string, string> property;
  edge e(property, src, target);
  uint64_t eid = cli.create_edge(target, e);
  cli.update_edge(target, eid, e);
  cli.remove_edge(target, eid);
}

// add_centrality_query, update_index, get_centrality, remove_centrality_query
TEST(graph_test, centrality) {
  graph cli(host(), port(), cluster_name(), timeout());
  string id = cli.create_node();
  preset_query q;
  cli.add_centrality_query(q);
  cli.update_index();
  cli.get_centrality(id, 0, q);
  cli.remove_centrality_query(q);
}

// add_shortest_path_query, get_shortest_path, remove_shortest_path_query
TEST(graph_test, add_shortest_path_query) {
  graph cli(host(), port(), cluster_name(), timeout());
  string id1 = cli.create_node(), id2 = cli.create_node();
  shortest_path_query req;
  req.source = id1;
  req.target = id2;
  req.max_hop = 10;
  preset_query q;
  cli.add_shortest_path_query(q);
  cli.get_shortest_path(req);
  cli.remove_shortest_path_query(q);
}

TEST(graph_test, update_index) {
  graph cli(host(), port(), cluster_name(), timeout());
  cli.update_index();
}

TEST(graph_test, clear) {
  graph cli(host(), port(), cluster_name(), timeout());
  ASSERT_TRUE(cli.clear());
}
