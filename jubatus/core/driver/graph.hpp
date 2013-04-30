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

#ifndef JUBATUS_CORE_DRIVER_GRAPH_HPP_
#define JUBATUS_CORE_DRIVER_GRAPH_HPP_

#include <string>
#include <utility>
#include <vector>
#include <pficommon/lang/shared_ptr.h>
#include "../graph/graph_base.hpp"
#include "../graph/graph_type.hpp"
#include "../graph/graph_wo_index.hpp"
#include "../framework/mixable.hpp"
#include "linear_function_mixer.hpp"
#include "mixable_weight_manager.hpp"

namespace jubatus {
namespace core {
namespace driver {

struct mixable_graph : public framework::mixable<
    jubatus::core::graph::graph_base,
    std::string> {
  void clear() {
  }

  std::string get_diff_impl() const {
    std::string diff;
    get_model()->get_diff(diff);
    return diff;
  }

  void mix_impl(
      const std::string& lhs,
      const std::string& rhs,
      std::string& mixed) const {
    mixed = lhs;
    jubatus::core::graph::graph_wo_index* graph =
        dynamic_cast<jubatus::core::graph::graph_wo_index*>(get_model().get());
    if (graph) {
      graph->mix(rhs, mixed);
    }
  }

  void put_diff_impl(const std::string& v) {
    get_model()->set_mixed_and_clear_diff(v);
  }
};

class graph {
 public:
  explicit graph(jubatus::core::graph::graph_base* graph_method);
  virtual ~graph();

  pfi::lang::shared_ptr<framework::mixable_holder> get_mixable_holder() const {
    return mixable_holder_;
  }

  jubatus::core::graph::graph_base* get_model() const {
    return graph_.get_model().get();
  }

  void create_node(jubatus::core::graph::node_id_t id);
  void update_node(
      jubatus::core::graph::node_id_t id,
      const jubatus::core::graph::property& p);
  void remove_node(jubatus::core::graph::node_id_t id);
  void create_edge(
      jubatus::core::graph::edge_id_t eid,
      jubatus::core::graph::node_id_t src,
      jubatus::core::graph::node_id_t target,
      const jubatus::core::graph::property& p);
  void update_edge(
      jubatus::core::graph::edge_id_t eid,
      const jubatus::core::graph::property& p);
  void remove_edge(jubatus::core::graph::edge_id_t eid);
  double get_centrality(
      jubatus::core::graph::node_id_t nid,
      jubatus::core::graph::centrality_type ct,
      const jubatus::core::graph::preset_query& q) const;
  std::vector<jubatus::core::graph::node_id_t> get_shortest_path(
      jubatus::core::graph::node_id_t src,
      jubatus::core::graph::node_id_t target,
      uint64_t max_hop,
      const jubatus::core::graph::preset_query &q) const;
  void add_centrality_query(const jubatus::core::graph::preset_query& q);
  void add_shortest_path_query(const jubatus::core::graph::preset_query& q);
  void remove_centrality_query(const jubatus::core::graph::preset_query& q);
  void remove_shortest_path_query(const jubatus::core::graph::preset_query& q);
  jubatus::core::graph::node_info get_node(
      jubatus::core::graph::node_id_t nid) const;
  jubatus::core::graph::edge_info get_edge(
      jubatus::core::graph::edge_id_t eid) const;
  void update_index();
  void clear();

  // internal apis used between servers
  void create_node_here(jubatus::core::graph::node_id_t id);
  void remove_global_node(jubatus::core::graph::node_id_t nid);
  void create_edge_here(
      jubatus::core::graph::edge_id_t eid,
      jubatus::core::graph::node_id_t src,
      jubatus::core::graph::node_id_t target,
      const jubatus::core::graph::property& p);

 private:
  pfi::lang::shared_ptr<framework::mixable_holder> mixable_holder_;

  mixable_graph graph_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_GRAPH_HPP_
