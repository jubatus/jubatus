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

#pragma once

#include <pficommon/data/unordered_map.h>
#include <pficommon/data/unordered_set.h>
#include "graph_base.hpp"

namespace jubatus{
namespace graph {

class graph_wo_index : public graph_base {
public:
  graph_wo_index();
  ~graph_wo_index();

  void clear();
  void create_node(node_id_t id);
  void create_global_node(node_id_t id);
  void remove_global_node(node_id_t id);
  void update_node(node_id_t id, const property& p);
  void remove_node(node_id_t id);

  void create_edge(edge_id_t eid, node_id_t src, node_id_t tgt);
  void update_edge(edge_id_t eid, const property& p);
  void remove_edge(edge_id_t eid);
  
  void add_cenrality_query(const preset_query&);
  void add_shortest_path_query(const preset_query&);
  void remove_cenrality_query(const preset_query&);
  void remove_shortest_path_query(const preset_query&);

  double centrality(node_id_t id, centrality_type ct, const preset_query&) const;
  void shortest_path(node_id_t src, node_id_t tgt, 
                     uint64_t max_hop, std::vector<node_id_t>& ret,
		     const preset_query&) const;
  
  void get_node(node_id_t id, node_info& ret) const;
  void get_edge(edge_id_t eid, edge_info& ret) const;

  void get_diff(std::string& diff)const;
  void set_mixed_and_clear_diff(const std::string& mixed);

  std::string type() const;

  void get_status(std::map<std::string, std::string>& status) const;
  void update_index();

  static void mix(const std::string& diff, std::string& mixed);

private:
  bool save_imp(std::ostream& os);
  bool load_imp(std::istream& is);

  static void remove_by_swap(std::vector<edge_id_t>& edges, edge_id_t eid);

  // =============================
  // improve me !
  template <class T, class U> U get(const pfi::data::unordered_map<T, U>& m, const T t) const {
    typename pfi::data::unordered_map<T, U>::const_iterator it = m.find(t);
    if (it == m.end()){
      return T();
    } else {
      return it->second;
    }
  }
  
  pfi::data::unordered_map<node_id_t, node_info> local_nodes_;
  pfi::data::unordered_map<edge_id_t, edge_info> local_edges_;
  pfi::data::unordered_map<node_id_t, uint8_t> global_nodes_; // value is dummy for serialization

  // centeralities
  double alpha_;
  pfi::data::unordered_map<node_id_t, eigen_vector_info> eigen_scores_;
  void get_diff_eigen_score(eigen_vector_diff& diff)const;
  void set_mixed_and_clear_diff_eigen_score(eigen_vector_mixed& mixed);
  static void mix(const eigen_vector_diff& diff, eigen_vector_mixed& mixed);

  // shortest pathes
  spt_mixed spts_;
  void may_set_landmark(node_id_t id);
  void get_diff_shortest_path_tree(spt_diff& diff)const;
  void set_mixed_and_clear_diff_shortest_path_tree(const spt_mixed& mixed);
  void update_spt();
  void update_spt_edges(spt_edges& se, node_id_t landmark, bool is_out);
  static void mix_spt(const shortest_path_tree& diff,
                      shortest_path_tree& mixed);
  static void mix(const spt_diff& diff, spt_mixed& mixed);
};

}
}
