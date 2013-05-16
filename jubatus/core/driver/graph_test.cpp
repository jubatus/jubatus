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

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../graph/graph.hpp"
#include "graph.hpp"
#include "test_util.hpp"

using std::string;
using std::vector;
using jubatus::core::graph::node_id_t;
using jubatus::core::graph::edge_id_t;
using jubatus::core::graph::edge_info;
using jubatus::core::graph::node_info;
using jubatus::core::graph::preset_query;

namespace jubatus {
namespace core {
namespace driver {

class graph_test : public ::testing::Test {
 protected:
  void SetUp() {
    core::graph::graph_wo_index::config config;
    config.alpha = 0.9f;
    config.landmark_num = 5;
    graph_.reset(new core::driver::graph(
          new core::graph::graph_wo_index(config)));
  }

  void TearDown() {
    graph_.reset();
  }

  pfi::lang::shared_ptr<core::driver::graph> graph_;
};

TEST_F(graph_test, simple) {
  node_id_t nid = 0;
  node_id_t nid0 = 1;
  graph_->create_node(nid);
  graph_->create_node(nid0);

  edge_id_t eid = 2;
  {
    core::graph::property p;
    p["hoge"] = "huga";
    p["name"] = "test0";
    graph_->update_node(nid, p);
    p["name"] = "testoooo";
    graph_->update_node(nid0, p);

    p["name"] = "edge_name_hoge";
    graph_->create_edge(eid, nid, nid0, p);
  }
  {
    string save_data;
    save_model(graph_->get_mixable_holder(), save_data);
    graph_->clear();
    load_model(graph_->get_mixable_holder(), save_data);
  }
  {
    node_info info = graph_->get_node(nid);
    ASSERT_EQ(0u, info.in_edges.size());

    // TODO(suma): is this correct?(before 1)
    EXPECT_EQ(1u, info.out_edges.size());

    ASSERT_EQ("huga", info.p["hoge"]);
    ASSERT_EQ("test0", info.p["name"]);
  }
  {
    edge_info info = graph_->get_edge(eid);
    ASSERT_EQ(nid , info.src);
    ASSERT_EQ(nid0, info.tgt);
    ASSERT_EQ(2u, info.p.size());
    ASSERT_EQ("edge_name_hoge", info.p["name"]);
  }

  {
    graph_->update_index();
  }

  {
    preset_query query;
    graph_->add_centrality_query(query);
    graph_->update_index();  // call manually in standalone mode
    ASSERT_LT(0.0, graph_->get_centrality(nid, core::graph::EIGENSCORE, query));
  }

  {
    node_id_t source_id = nid;
    node_id_t target_id = nid0;
    uint64_t max_hop = 10;
    {  // TODO(kuenishi)
      // preset_query query;
      // graph_->add_shortest_path_query(query);
      // vector<node_id_t> path = graph_->get_shortest_path(
      //     source_id, target_id, max_hop, query);
      // ASSERT_EQ(1u, path.size());
    }
    {
      preset_query query;
      graph_->add_shortest_path_query(query);
      vector<node_id_t> path = graph_->get_shortest_path(
          source_id, target_id, max_hop, query);
      ASSERT_EQ(0u, path.size());
    }
  }

  {
    preset_query query;
    graph_->add_centrality_query(query);
    graph_->add_shortest_path_query(query);
    graph_->remove_centrality_query(query);
    graph_->remove_shortest_path_query(query);
  }
}

}  // driver namespace
}  // core namespace
}  // jubatus namespace
