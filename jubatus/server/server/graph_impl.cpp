// This file is auto-generated from graph.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/shared_ptr.h>

#include "../framework.hpp"
#include "graph_server.hpp"
#include "graph_serv.hpp"

namespace jubatus {
namespace server {

class graph_impl_ : public graph<graph_impl_> {
 public:
  explicit graph_impl_(const jubatus::server::framework::server_argv& a):
    graph<graph_impl_>(a.timeout),
    p_(new jubatus::server::framework::server_helper<graph_serv>(a, true)) {
  }
  std::string get_config(std::string name) {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  std::string create_node(std::string name) {
    NOLOCK_(p_);
    return get_p()->create_node();
  }

  bool remove_node(std::string name, std::string node_id) {
    JWLOCK_(p_);
    return get_p()->remove_node(node_id);
  }

  bool update_node(std::string name, std::string node_id, std::map<std::string,
       std::string> property) {
    JWLOCK_(p_);
    return get_p()->update_node(node_id, property);
  }

  uint64_t create_edge(std::string name, std::string node_id, edge e) {
    NOLOCK_(p_);
    return get_p()->create_edge(node_id, e);
  }

  bool update_edge(std::string name, std::string node_id, uint64_t edge_id,
       edge e) {
    JWLOCK_(p_);
    return get_p()->update_edge(node_id, edge_id, e);
  }

  bool remove_edge(std::string name, std::string node_id, uint64_t edge_id) {
    JWLOCK_(p_);
    return get_p()->remove_edge(node_id, edge_id);
  }

  double get_centrality(std::string name, std::string node_id,
       int32_t centrality_type, preset_query query) {
    JRLOCK_(p_);
    return get_p()->get_centrality(node_id, centrality_type, query);
  }

  bool add_centrality_query(std::string name, preset_query query) {
    JWLOCK_(p_);
    return get_p()->add_centrality_query(query);
  }

  bool add_shortest_path_query(std::string name, preset_query query) {
    JWLOCK_(p_);
    return get_p()->add_shortest_path_query(query);
  }

  bool remove_centrality_query(std::string name, preset_query query) {
    JWLOCK_(p_);
    return get_p()->remove_centrality_query(query);
  }

  bool remove_shortest_path_query(std::string name, preset_query query) {
    JWLOCK_(p_);
    return get_p()->remove_shortest_path_query(query);
  }

  std::vector<std::string> get_shortest_path(std::string name,
       shortest_path_query query) {
    JRLOCK_(p_);
    return get_p()->get_shortest_path(query);
  }

  bool update_index(std::string name) {
    JWLOCK_(p_);
    return get_p()->update_index();
  }

  bool clear(std::string name) {
    JWLOCK_(p_);
    return get_p()->clear();
  }

  node get_node(std::string name, std::string node_id) {
    JRLOCK_(p_);
    return get_p()->get_node(node_id);
  }

  edge get_edge(std::string name, std::string node_id, uint64_t edge_id) {
    JRLOCK_(p_);
    return get_p()->get_edge(node_id, edge_id);
  }

  bool save(std::string name, std::string id) {
    JWLOCK_(p_);
    return get_p()->save(id);
  }

  bool load(std::string name, std::string id) {
    JWLOCK_(p_);
    return get_p()->load(id);
  }

  std::map<std::string, std::map<std::string, std::string> > get_status(
      std::string name) {
    JRLOCK_(p_);
    return p_->get_status();
  }

  bool create_node_here(std::string name, std::string node_id) {
    JWLOCK_(p_);
    return get_p()->create_node_here(node_id);
  }

  bool remove_global_node(std::string name, std::string node_id) {
    JWLOCK_(p_);
    return get_p()->remove_global_node(node_id);
  }

  bool create_edge_here(std::string name, uint64_t edge_id, edge e) {
    JWLOCK_(p_);
    return get_p()->create_edge_here(edge_id, e);
  }
  int run() { return p_->start(*this); }
  pfi::lang::shared_ptr<graph_serv> get_p() { return p_->server(); }

 private:
  pfi::lang::shared_ptr<jubatus::server::framework::server_helper<graph_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::graph_impl_>
      (argc, argv, "graph");
}
