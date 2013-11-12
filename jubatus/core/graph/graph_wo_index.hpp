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

#ifndef JUBATUS_CORE_GRAPH_GRAPH_WO_INDEX_HPP_
#define JUBATUS_CORE_GRAPH_GRAPH_WO_INDEX_HPP_

#include <map>
#include <string>
#include <vector>

#include "jubatus/util/data/unordered_map.h"
#include "jubatus/util/data/unordered_set.h"
#include "jubatus/util/data/serialization.h"
#include "jubatus/util/lang/enable_shared_from_this.h"
#include "jubatus/util/lang/shared_ptr.h"

#include "../common/unordered_map.hpp"
#include "../framework/mixable.hpp"
#include "graph_base.hpp"

namespace jubatus {
namespace core {
namespace graph {

class graph_wo_index
    : public graph_base,
      public jubatus::util::lang::enable_shared_from_this<graph_wo_index> {
 public:
  struct diff_type {
    eigen_vector_query_diff eigen_vector_query;
    spt_query_diff spt_query;

    void clear() {
      eigen_vector_query.clear();
      spt_query.clear();
    }

    MSGPACK_DEFINE(eigen_vector_query, spt_query);

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & MEMBER(eigen_vector_query) & MEMBER(spt_query);
    }
  };

  struct config {
    config()
        : alpha(0.9),
          landmark_num(5) {
    }

    double alpha;
    int landmark_num;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & NAMED_MEMBER("damping_factor", alpha) & MEMBER(landmark_num);
    }
  };

  explicit graph_wo_index(const config& config);
  graph_wo_index();
  ~graph_wo_index();

  void alpha(double a);

  void clear();
  void create_node(node_id_t id);
  void create_global_node(node_id_t id);
  void remove_global_node(node_id_t id);
  void update_node(node_id_t id, const property& p);
  void remove_node(node_id_t id);

  void create_edge(edge_id_t eid, node_id_t src, node_id_t tgt);
  void update_edge(edge_id_t eid, const property& p);
  void remove_edge(edge_id_t eid);

  void add_centrality_query(const preset_query&);
  void add_shortest_path_query(const preset_query&);
  void remove_centrality_query(const preset_query&);
  void remove_shortest_path_query(const preset_query&);

  double centrality(
      node_id_t id,
      centrality_type ct,
      const preset_query&) const;

  void shortest_path(
      node_id_t src,
      node_id_t tgt,
      uint64_t max_hop,
      std::vector<node_id_t>& ret,
      const preset_query&) const;

  void get_node(node_id_t id, node_info& ret) const;
  void get_edge(edge_id_t eid, edge_info& ret) const;

  void get_diff(diff_type& diff) const;
  void set_mixed_and_clear_diff(const diff_type& mixed);

  std::string type() const;

  void get_status(std::map<std::string, std::string>& status) const;
  void update_index();

  void mix(const diff_type& diff, diff_type& mixed);

  void register_mixables_to_holder(framework::mixable_holder& holder) const;

  void pack(msgpack::packer<msgpack::sbuffer>& packer) const;
  void unpack(msgpack::object o);

  MSGPACK_DEFINE(local_nodes_, local_edges_,
      global_nodes_, eigen_scores_, spts_);

 private:
  typedef jubatus::util::data::unordered_map<node_id_t, node_info>
    node_info_map;
  typedef jubatus::util::data::unordered_map<edge_id_t, edge_info>
    edge_info_map;

  static void remove_by_swap(std::vector<edge_id_t>& edges, edge_id_t eid);

  node_info_map local_nodes_;
  edge_info_map local_edges_;

  // value is dummy for serialization
  jubatus::util::data::unordered_map<node_id_t, uint8_t> global_nodes_;

  // centeralities
  static void mix(
      const eigen_vector_query_diff& diff,
      eigen_vector_query_diff& mixed);

  void get_diff_eigen_score(eigen_vector_query_diff& diff) const;
  void set_mixed_and_clear_diff_eigen_score(
      const eigen_vector_query_diff& mixed);

  eigen_vector_query_diff eigen_scores_;

  // shortest pathes
  static void mix_spt(
      const shortest_path_tree& diff,
      shortest_path_tree& mixed);
  static void mix(
      const spt_query_diff& diff,
      spt_query_diff& mixed,
      size_t landmark_num);

  void may_set_landmark(node_id_t id);

  void get_diff_shortest_path_tree(spt_query_diff& diff) const;

  void set_mixed_and_clear_diff_shortest_path_tree(
      const spt_query_diff& mixed);

  void update_spt();

  void update_spt_edges(
      const preset_query& query,
      spt_edges& se,
      node_id_t landmark,
      bool is_out);

  void update_spt_node(
      const preset_query& query,
      const std::vector<edge_id_t>& edges,
      spt_edges& se,
      bool is_out);

  bool is_node_matched_to_query(const preset_query& query, node_id_t id) const;

  spt_query_diff spts_;

  config config_;
};

typedef framework::delegating_mixable<graph_wo_index, graph_wo_index::diff_type>
    mixable_graph_wo_index;

}  // namespace graph
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_GRAPH_GRAPH_WO_INDEX_HPP_
