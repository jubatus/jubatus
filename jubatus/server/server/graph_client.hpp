// This file is auto-generated from graph.idl(0.4.3-274-g83cbcee) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_GRAPH_CLIENT_HPP_
#define JUBATUS_SERVER_SERVER_GRAPH_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <jubatus/client/common/client.hpp>
#include "jubatus/core/fv_converter/datum.hpp"
#include "graph_types.hpp"

namespace jubatus {
namespace client {

class graph : public jubatus::client::common::client {
 public:
  graph(const std::string& host, uint64_t port, const std::string& name,
      unsigned int timeout_sec)
      : client(host, port, name, timeout_sec) {
  }

  std::string create_node() {
    msgpack::rpc::future f = c_.call("create_node", name_);
    return f.get<std::string>();
  }

  bool remove_node(const std::string& node_id) {
    msgpack::rpc::future f = c_.call("remove_node", name_, node_id);
    return f.get<bool>();
  }

  bool update_node(const std::string& node_id, const std::map<std::string,
      std::string>& property) {
    msgpack::rpc::future f = c_.call("update_node", name_, node_id, property);
    return f.get<bool>();
  }

  uint64_t create_edge(const std::string& node_id, const edge& e) {
    msgpack::rpc::future f = c_.call("create_edge", name_, node_id, e);
    return f.get<uint64_t>();
  }

  bool update_edge(const std::string& node_id, uint64_t edge_id,
      const edge& e) {
    msgpack::rpc::future f = c_.call("update_edge", name_, node_id, edge_id, e);
    return f.get<bool>();
  }

  bool remove_edge(const std::string& node_id, uint64_t edge_id) {
    msgpack::rpc::future f = c_.call("remove_edge", name_, node_id, edge_id);
    return f.get<bool>();
  }

  double get_centrality(const std::string& node_id, int32_t centrality_type,
      const jubatus::core::graph::preset_query& query) {
    msgpack::rpc::future f = c_.call("get_centrality", name_, node_id,
        centrality_type, query);
    return f.get<double>();
  }

  bool add_centrality_query(const jubatus::core::graph::preset_query& query) {
    msgpack::rpc::future f = c_.call("add_centrality_query", name_, query);
    return f.get<bool>();
  }

  bool add_shortest_path_query(
      const jubatus::core::graph::preset_query& query) {
    msgpack::rpc::future f = c_.call("add_shortest_path_query", name_, query);
    return f.get<bool>();
  }

  bool remove_centrality_query(
      const jubatus::core::graph::preset_query& query) {
    msgpack::rpc::future f = c_.call("remove_centrality_query", name_, query);
    return f.get<bool>();
  }

  bool remove_shortest_path_query(
      const jubatus::core::graph::preset_query& query) {
    msgpack::rpc::future f = c_.call("remove_shortest_path_query", name_,
        query);
    return f.get<bool>();
  }

  std::vector<std::string> get_shortest_path(const shortest_path_query& query) {
    msgpack::rpc::future f = c_.call("get_shortest_path", name_, query);
    return f.get<std::vector<std::string> >();
  }

  bool update_index() {
    msgpack::rpc::future f = c_.call("update_index", name_);
    return f.get<bool>();
  }

  bool clear() {
    msgpack::rpc::future f = c_.call("clear", name_);
    return f.get<bool>();
  }

  jubatus::core::graph::node_info get_node(const std::string& node_id) {
    msgpack::rpc::future f = c_.call("get_node", name_, node_id);
    return f.get<jubatus::core::graph::node_info>();
  }

  edge get_edge(const std::string& node_id, uint64_t edge_id) {
    msgpack::rpc::future f = c_.call("get_edge", name_, node_id, edge_id);
    return f.get<edge>();
  }

  bool create_node_here(const std::string& node_id) {
    msgpack::rpc::future f = c_.call("create_node_here", name_, node_id);
    return f.get<bool>();
  }

  bool remove_global_node(const std::string& node_id) {
    msgpack::rpc::future f = c_.call("remove_global_node", name_, node_id);
    return f.get<bool>();
  }

  bool create_edge_here(uint64_t edge_id, const edge& e) {
    msgpack::rpc::future f = c_.call("create_edge_here", name_, edge_id, e);
    return f.get<bool>();
  }
};

}  // namespace client
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_GRAPH_CLIENT_HPP_
