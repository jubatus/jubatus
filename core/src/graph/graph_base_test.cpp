// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "graph_base.hpp"

namespace jubatus {
namespace core {
namespace graph {

class graph_impl : public graph_base {
 public:
  graph_impl() :
      graph_base() {
  }

  void clear() {
  }

  void create_node(node_id_t id) {
  }

  void create_global_node(node_id_t id) {
  }

  void remove_global_node(node_id_t id) {
  }

  void update_node(node_id_t id, const property& p) {
  }

  void remove_node(node_id_t id) {
  }

  void create_edge(edge_id_t eid, node_id_t src, node_id_t tgt) {
  }

  void update_edge(edge_id_t eid, const property& p) {
  }

  void remove_edge(edge_id_t eid) {
  }


  double centrality(node_id_t id, centrality_type ct) const {
    return 0.0;
  }

  void shortest_path(
      node_id_t src,
      node_id_t tgt,
      uint64_t max_hop,
      std::vector<node_id_t>& ret) const {
  }

  void get_node(node_id_t id, node_info& ret) const {
  }

  void get_edge(node_id_t id, edge_info& ret) const {
  }

  std::string type() const {
    return "";
  }

  void get_status(std::map<std::string, std::string>& status) const {
  }

  void update_index() {
  }

  void get_diff(std::string& diff) {
  }

  void set_mixed_and_clear_diff(std::string& mixed) {
  }
};

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
