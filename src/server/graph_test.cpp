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

#include <string>
#include <vector>
#include <map>

#include <gtest/gtest.h>

#include "test_util.hpp"
#include "graph_client.hpp"

using jubatus::client::graph;

static const int PORT = 65431;

namespace jubatus {

typedef uint64_t edge_id_t;
typedef std::string node_id;
typedef int centrality_type;

}  // namespace jubatus

namespace {

class graph_test : public ::testing::Test {
 protected:
  pid_t child_;

  graph_test() {
    child_ = fork_process("graph", PORT, "./test_input/config.graph.json");
  }

  virtual ~graph_test() {
    kill_process(child_);
  }

  virtual void restart_process() {
    kill_process(this->child_);
    this->child_ = fork_process("graph",
                                PORT,
                                "./test_input/config.graph.json");
  }
};

TEST_F(graph_test, simple) {
  jubatus::node_id nid = "0";
  jubatus::node_id nid0 = "1";
  graph c("localhost", PORT, 10);
  {
    c.clear("");
    nid = c.create_node("");
    nid0 = c.create_node("");

    c.get_config("");
  };
  jubatus::edge_id_t eid = 0;
  {
    std::map<std::string, std::string> p;
    p["hoge"] = "huga";
    p["name"] = "test0";
    c.update_node("", nid, p);
    p["name"] = "testoooo";
    c.update_node("", nid0, p);

    p["name"] = "edge_name_hoge";
    jubatus::edge ei;
    ei.source = nid;
    ei.target = nid0;
    ei.property = p;
    eid = c.create_edge("", nid, ei);
    c.create_edge("", nid0, ei);  // TODO(kmaehashi): do we need this? release?
  }
  {
    c.save("", "test");
    c.clear("");
    c.load("", "test");
  }
  {
    jubatus::node i = c.get_node("", nid);
    ASSERT_EQ(0u, i.in_edges.size());

    // TODO(suma): is this correct?(before 1)
    EXPECT_EQ(2u, i.out_edges.size());

    ASSERT_EQ("huga", i.property["hoge"]);
    ASSERT_EQ("test0", i.property["name"]);
  }
  {
    // eid = 0 , is nothing
    jubatus::edge i = c.get_edge("", nid, eid);
    ASSERT_EQ(nid , i.source);
    ASSERT_EQ(nid0, i.target);
    ASSERT_EQ(2u, i.property.size());
    ASSERT_EQ("edge_name_hoge", i.property["name"]);
  }

  {
    c.get_status("");
    c.update_index("");
  }

  {
    jubatus::preset_query q;
    c.add_centrality_query("", q);
    c.update_index("");  // call manually in standalone mode
    double cent = c.get_centrality("", nid, 0, q);
    ASSERT_LT(0.0, cent);
  }

  {
    jubatus::shortest_path_query req;
    req.source = nid;
    req.target = nid0;
    {  // TODO(kuenishi)
      req.max_hop = 10;
      jubatus::preset_query q;
      // c.add_shortest_path_query("", q);
      // std::vector<jubatus::node_id> path = c.get_shortest_path("", req);
      // ASSERT_EQ(1u, path.size());
    }
    {
      req.max_hop = 0;
      jubatus::preset_query q;
      c.add_shortest_path_query("", q);
      std::vector<jubatus::node_id> path = c.get_shortest_path("", req);
      ASSERT_EQ(0u, path.size());
    }
  }

  {
    jubatus::preset_query q;
    c.add_centrality_query("", q);
    c.add_shortest_path_query("", q);
    c.remove_centrality_query("", q);
    c.remove_shortest_path_query("", q);
  }
}

TEST_F(graph_test, api_get_client) {
  jubatus::client::graph cli("localhost", PORT, 10);
  string to_get = cli.get_config("");

  msgpack::rpc::client& conn = cli.get_client();
  EXPECT_NO_THROW(conn.close());
}

}  // namespace
