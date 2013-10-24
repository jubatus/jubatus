// This file is auto-generated from graph.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_GRAPH_CLIENT_HPP_
#define JUBATUS_CLIENT_GRAPH_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <jubatus/msgpack/rpc/client.h>
#include "graph_types.hpp"

namespace jubatus {
namespace graph {
namespace client {

class graph {
 public:
  graph(const std::string& host, uint64_t port, double timeout_sec)
      : c_(host, port) {
    c_.set_timeout(timeout_sec);
  }

  std::string get_config(const std::string& name) {
    msgpack::rpc::future f = c_.call("get_config", name);
    return f.get<std::string>();
  }

  std::string create_node(const std::string& name) {
    msgpack::rpc::future f = c_.call("create_node", name);
    return f.get<std::string>();
  }

  bool remove_node(const std::string& name, const std::string& node_id) {
    msgpack::rpc::future f = c_.call("remove_node", name, node_id);
    return f.get<bool>();
  }

  bool update_node(const std::string& name, const std::string& node_id,
      const std::map<std::string, std::string>& property) {
    msgpack::rpc::future f = c_.call("update_node", name, node_id, property);
    return f.get<bool>();
  }

  uint64_t create_edge(const std::string& name, const std::string& node_id,
      const edge& e) {
    msgpack::rpc::future f = c_.call("create_edge", name, node_id, e);
    return f.get<uint64_t>();
  }

  bool update_edge(const std::string& name, const std::string& node_id,
      uint64_t edge_id, const edge& e) {
    msgpack::rpc::future f = c_.call("update_edge", name, node_id, edge_id, e);
    return f.get<bool>();
  }

  bool remove_edge(const std::string& name, const std::string& node_id,
      uint64_t edge_id) {
    msgpack::rpc::future f = c_.call("remove_edge", name, node_id, edge_id);
    return f.get<bool>();
  }

  double get_centrality(const std::string& name, const std::string& node_id,
      int32_t centrality_type, const preset_query& query) {
    msgpack::rpc::future f = c_.call("get_centrality", name, node_id,
        centrality_type, query);
    return f.get<double>();
  }

  bool add_centrality_query(const std::string& name,
      const preset_query& query) {
    msgpack::rpc::future f = c_.call("add_centrality_query", name, query);
    return f.get<bool>();
  }

  bool add_shortest_path_query(const std::string& name,
      const preset_query& query) {
    msgpack::rpc::future f = c_.call("add_shortest_path_query", name, query);
    return f.get<bool>();
  }

  bool remove_centrality_query(const std::string& name,
      const preset_query& query) {
    msgpack::rpc::future f = c_.call("remove_centrality_query", name, query);
    return f.get<bool>();
  }

  bool remove_shortest_path_query(const std::string& name,
      const preset_query& query) {
    msgpack::rpc::future f = c_.call("remove_shortest_path_query", name, query);
    return f.get<bool>();
  }

  std::vector<std::string> get_shortest_path(const std::string& name,
      const shortest_path_query& query) {
    msgpack::rpc::future f = c_.call("get_shortest_path", name, query);
    return f.get<std::vector<std::string> >();
  }

  bool update_index(const std::string& name) {
    msgpack::rpc::future f = c_.call("update_index", name);
    return f.get<bool>();
  }

  bool clear(const std::string& name) {
    msgpack::rpc::future f = c_.call("clear", name);
    return f.get<bool>();
  }

  node get_node(const std::string& name, const std::string& node_id) {
    msgpack::rpc::future f = c_.call("get_node", name, node_id);
    return f.get<node>();
  }

  edge get_edge(const std::string& name, const std::string& node_id,
      uint64_t edge_id) {
    msgpack::rpc::future f = c_.call("get_edge", name, node_id, edge_id);
    return f.get<edge>();
  }

  bool save(const std::string& name, const std::string& id) {
    msgpack::rpc::future f = c_.call("save", name, id);
    return f.get<bool>();
  }

  bool load(const std::string& name, const std::string& id) {
    msgpack::rpc::future f = c_.call("load", name, id);
    return f.get<bool>();
  }

  std::map<std::string, std::map<std::string, std::string> > get_status(
      const std::string& name) {
    msgpack::rpc::future f = c_.call("get_status", name);
    return f.get<std::map<std::string, std::map<std::string, std::string> > >();
  }

  bool create_node_here(const std::string& name, const std::string& node_id) {
    msgpack::rpc::future f = c_.call("create_node_here", name, node_id);
    return f.get<bool>();
  }

  bool remove_global_node(const std::string& name, const std::string& node_id) {
    msgpack::rpc::future f = c_.call("remove_global_node", name, node_id);
    return f.get<bool>();
  }

  bool create_edge_here(const std::string& name, uint64_t edge_id,
      const edge& e) {
    msgpack::rpc::future f = c_.call("create_edge_here", name, edge_id, e);
    return f.get<bool>();
  }

  msgpack::rpc::client& get_client() {
    return c_;
  }

 private:
  msgpack::rpc::client c_;
};

}  // namespace client
}  // namespace graph
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_GRAPH_CLIENT_HPP_
