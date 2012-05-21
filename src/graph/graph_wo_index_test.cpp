// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
#include <stdexcept>
#include <pficommon/lang/cast.h>
#include "graph_wo_index.hpp"

using namespace std;

namespace jubatus{
namespace graph {

TEST(graph_wo_index, none){
  graph_wo_index g;
  preset_query q;

  g.clear();
  ASSERT_EQ("graph_wo_index", g.type());

  EXPECT_THROW(g.update_node(7, property()), runtime_error);
  EXPECT_THROW(g.remove_global_node(7), runtime_error);
  EXPECT_THROW(g.update_edge(8, property()), runtime_error);
  EXPECT_THROW(g.remove_edge(8), runtime_error);
  EXPECT_THROW(g.centrality(9, EIGENSCORE, q), runtime_error);
  vector<node_id_t> ret;
  EXPECT_THROW(g.shortest_path(10, 11, 1000, ret, q), runtime_error);

  {
    map<string, string> status;
    g.get_status(status);
    ASSERT_EQ("0", status["local_node_num"]);
    ASSERT_EQ("0", status["local_edge_num"]);
    ASSERT_EQ("0", status["global_node_num"]);
  }

  g.create_node(777);
  g.create_global_node(777);
  {
    map<string, string> status;
    g.get_status(status);
    ASSERT_EQ("1", status["local_node_num"]);
    ASSERT_EQ("1", status["global_node_num"]);
  }
  g.create_global_node(888);
  g.create_node(888);
  g.create_global_node(999);
  {
    map<string, string> status;
    g.get_status(status);
    ASSERT_EQ("2", status["local_node_num"]);
    ASSERT_EQ("3", status["global_node_num"]);
  }

  g.create_edge(10000, 777, 999);
  g.create_edge(10001, 777, 888);
  g.create_edge(10002, 777, 999);
  EXPECT_THROW(g.create_edge(10002, 4, 5), runtime_error);
  EXPECT_THROW(g.create_edge(10002, 999, 5), runtime_error);
  {
    map<string, string> status;
    g.get_status(status);
    ASSERT_EQ("2", status["local_node_num"]);
    ASSERT_EQ("3", status["local_edge_num"]);
    ASSERT_EQ("3", status["global_node_num"]);
  }

  {
    edge_info ei;
    g.get_edge(10000, ei);
    ASSERT_EQ(777, ei.src);
    ASSERT_EQ(999, ei.tgt);
  }

  {
    node_info ni;
    g.get_node(777, ni);
    ASSERT_EQ(0, ni.in_edges.size());
    ASSERT_EQ(3, ni.out_edges.size());
  }
  {
    node_info ni;
    g.get_node(888, ni);
    ASSERT_EQ(1, ni.in_edges.size());
    ASSERT_EQ(0, ni.out_edges.size());
    ASSERT_EQ(10001, ni.in_edges[0]);
  }

  EXPECT_THROW(g.remove_node(5), runtime_error);
  EXPECT_THROW(g.remove_node(999), runtime_error);
  EXPECT_THROW(g.remove_edge(5), runtime_error);
  EXPECT_THROW(g.remove_node(777), runtime_error); // edge is associated
  EXPECT_THROW(g.remove_node(888), runtime_error); // edge is associated

  g.remove_edge(10000);
  g.remove_edge(10001);
  g.remove_node(888);
  g.remove_global_node(888);
  {
    map<string, string> status;
    g.get_status(status);
    ASSERT_EQ("1", status["local_node_num"]);
    ASSERT_EQ("1", status["local_edge_num"]);
    ASSERT_EQ("2", status["global_node_num"]);
  }

  g.update_index();
}

TEST(graph, random){
  graph_wo_index g;

  const uint64_t node_num = 1000;
  uint64_t local_node_num = 0;
  set<uint64_t> local_ids;
  for (uint64_t i = 0; i < node_num; ++i){
    g.create_global_node(i);
    if ((rand() % 10) == 0){
      g.create_node(i);
      local_ids.insert(i);
      ++local_node_num;
    }
  }
  
  const uint64_t edge_num = 10000;
  uint64_t edge_added_num = 0;
  for (uint64_t i = 0; i < edge_num; ++i){
    uint64_t src = rand() % node_num;
    uint64_t tgt = rand() % node_num;
    if (src == tgt) continue;
    if (local_ids.count(src) == 0 && local_ids.count(tgt) == 0) continue;
    g.create_edge(i, src, tgt);
    ++edge_added_num;
  }
  
  map<string, string> status;
  g.get_status(status);
  ASSERT_EQ(pfi::lang::lexical_cast<string>(local_node_num), status["local_node_num"]);
  ASSERT_EQ(pfi::lang::lexical_cast<string>(node_num), status["global_node_num"]);
  ASSERT_EQ(pfi::lang::lexical_cast<string>(edge_added_num), status["local_edge_num"]);

  
}

TEST(graph, mix){
  graph_wo_index g;
  string diff;
  g.get_diff(diff);
  string mixed;
  g.mix(diff, mixed);
  g.set_mixed_and_clear_diff(mixed);
}


}
}
