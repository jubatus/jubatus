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

#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../common/global_id_generator.hpp"
#include "../framework.hpp"
#include "../framework/mixer/mixer.hpp"
#include "../framework/server_base.hpp"
#include "../graph/graph_base.hpp"
#include "../graph/graph_wo_index.hpp"
#include "graph_types.hpp"

namespace jubatus {
namespace server {

typedef uint64_t edge_id_t;
typedef std::string node_id;
typedef int centrality_type;

struct mixable_graph : public framework::mixable<
    jubatus::graph::graph_base,
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
    jubatus::graph::graph_wo_index* graph =
        dynamic_cast<jubatus::graph::graph_wo_index*>(get_model().get());
    if (graph) {
      graph->mix(rhs, mixed);
    }
  }

  void put_diff_impl(const std::string& v) {
    get_model()->set_mixed_and_clear_diff(v);
  }
};

class graph_serv : public framework::server_base {
 public:
  graph_serv(
      const framework::server_argv& a,
      const common::cshared_ptr<common::lock_service>& zk);
  virtual ~graph_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  pfi::lang::shared_ptr<framework::mixable_holder> get_mixable_holder() const {
    return mixable_holder_;
  }

  bool set_config(const std::string& config);
  std::string get_config() const;
  void check_set_config() const;

  void get_status(status_t& status) const;

  std::string create_node();

  bool update_node(
      const std::string& nid,
      const std::map<std::string, std::string>& p);

  bool remove_node(const std::string& nid);

  edge_id_t create_edge(const std::string& nid, const edge&);

  bool update_edge(const std::string& nid, edge_id_t, const edge&);

  bool remove_edge(const std::string& nid, const edge_id_t& e);

  double get_centrality(
      const std::string& nid,
      const centrality_type& ct,
      const preset_query& q) const;

  std::vector<node_id> get_shortest_path(const shortest_path_query& r) const;

  bool add_centrality_query(const preset_query& q);

  bool add_shortest_path_query(const preset_query& q);

  bool remove_centrality_query(const preset_query& q);

  bool remove_shortest_path_query(const preset_query& q);

  bool update_index();

  bool clear();

  node get_node(const std::string& nid) const;

  edge get_edge(const std::string& nid, const edge_id_t& e) const;

  // internal apis used between servers
  bool create_node_here(const std::string& nid);
  bool create_global_node(const std::string& nid);
  bool remove_global_node(const std::string& nid);

  bool create_edge_here(edge_id_t eid, const edge& ei);

 private:
  void selective_create_node_(
      const std::pair<std::string, int>& target,
      const std::string nid_str);

  void find_from_cht_(
      const std::string& key,
      size_t n,
      std::vector<std::pair<std::string, int> >& out);
  void get_members_(std::vector<std::pair<std::string, int> >& ret);

  common::cshared_ptr<common::lock_service> zk_;

  pfi::lang::scoped_ptr<framework::mixer::mixer> mixer_;
  pfi::lang::shared_ptr<framework::mixable_holder> mixable_holder_;
  common::global_id_generator idgen_;

  std::string config_;
  mixable_graph g_;
};

}  // namespace server
}  // namespace jubatus
