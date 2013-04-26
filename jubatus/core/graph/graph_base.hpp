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

#ifndef JUBATUS_CORE_GRAPH_GRAPH_BASE_HPP_
#define JUBATUS_CORE_GRAPH_GRAPH_BASE_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "graph_type.hpp"

namespace jubatus {
namespace core {
namespace graph {

class graph_base {
 public:
  graph_base();
  virtual ~graph_base();

  virtual void clear() = 0;
  virtual void create_node(node_id_t id) = 0;
  virtual void create_global_node(node_id_t id) = 0;
  virtual void remove_global_node(node_id_t id) = 0;
  virtual void update_node(node_id_t id, const property& p) = 0;
  virtual void remove_node(node_id_t id) = 0;
  virtual void create_edge(edge_id_t eid, node_id_t src, node_id_t tgt) = 0;
  virtual void update_edge(edge_id_t eid, const property& p) = 0;
  virtual void remove_edge(edge_id_t eid) = 0;

  virtual void add_centrality_query(const preset_query&) = 0;
  virtual void add_shortest_path_query(const preset_query&) = 0;
  virtual void remove_centrality_query(const preset_query&) = 0;
  virtual void remove_shortest_path_query(const preset_query&) = 0;

  virtual double centrality(
      node_id_t id,
      centrality_type ct,
      const preset_query&) const = 0;

  virtual void shortest_path(
      node_id_t src,
      node_id_t tgt,
      uint64_t max_hop,
      std::vector<node_id_t>& ret,
      const preset_query&) const = 0;

  virtual void get_node(node_id_t id, node_info& ret) const = 0;
  virtual void get_edge(edge_id_t id, edge_info& ret) const = 0;

  virtual std::string type() const = 0;

  virtual void get_status(std::map<std::string, std::string>& status) const = 0;
  virtual void update_index() = 0;

  virtual void get_diff(std::string& diff) const = 0;
  virtual void set_mixed_and_clear_diff(const std::string& mixed) = 0;

  void save(std::ostream&);
  void load(std::istream&);

 protected:
  virtual bool save_imp(std::ostream& os) = 0;
  virtual bool load_imp(std::istream& is) = 0;
};

}  // namespace graph
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_GRAPH_GRAPH_BASE_HPP_
