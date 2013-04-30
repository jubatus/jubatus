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

#ifndef JUBATUS_SERVER_SERVER_GRAPH_SERV_HPP_
#define JUBATUS_SERVER_SERVER_GRAPH_SERV_HPP_

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <pficommon/lang/shared_ptr.h>

#include "jubatus/core/driver/graph.hpp"
#include "../common/global_id_generator_base.hpp"
#include "../common/lock_service.hpp"
#include "../framework/server_base.hpp"
#include "graph_types.hpp"

namespace jubatus {
namespace server {

typedef uint64_t edge_id_t;
typedef std::string node_id;
typedef int centrality_type;

class graph_serv : public framework::server_base {
 public:
  graph_serv(
      const framework::server_argv& a,
      const pfi::lang::shared_ptr<common::lock_service>& zk);
  virtual ~graph_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  pfi::lang::shared_ptr<core::framework::mixable_holder>
    get_mixable_holder() const {
    return graph_->get_mixable_holder();
  }

  bool set_config(const std::string& config);
  std::string get_config() const;

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
  edge get_edge(const std::string& nid, edge_id_t e) const;

  // internal apis used between servers
  bool create_node_here(const std::string& nid);
  bool create_global_node(const std::string& nid);
  bool remove_global_node(const std::string& nid);

  bool create_edge_here(edge_id_t eid, const edge& ei);

 private:
  void check_set_config() const;

  void selective_create_node_(
      const std::pair<std::string, int>& target,
      const std::string nid_str);

  void find_from_cht_(
      const std::string& key,
      size_t n,
      std::vector<std::pair<std::string, int> >& out);
  void get_members_(std::vector<std::pair<std::string, int> >& ret);


  pfi::lang::shared_ptr<framework::mixer::mixer> mixer_;
  pfi::lang::shared_ptr<core::driver::graph> graph_;
  std::string config_;

  pfi::lang::shared_ptr<common::lock_service> zk_;
  pfi::lang::shared_ptr<common::global_id_generator_base> idgen_;
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_GRAPH_SERV_HPP_
