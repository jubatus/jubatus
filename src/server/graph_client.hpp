
// This file is auto-generated from graph.idl
// *** DO NOT EDIT ***

#ifndef GRAPH_CLIENT_HPP_
#define GRAPH_CLIENT_HPP_


#include "graph_types.hpp"
#include <jubatus/msgpack/rpc/client.h>


namespace jubatus {

namespace client {

class graph {
public:
  graph(const std::string &host, uint64_t port, double timeout_sec)
    : c_(host, port) {
    c_.set_timeout( timeout_sec );
  }

  std::string create_node(std::string name) {
    return c_.call("create_node", name).get<std::string>();
  }

  int32_t remove_node(std::string name, std::string nid) {
    return c_.call("remove_node", name, nid).get<int32_t>();
  }

  int32_t update_node(std::string name, std::string nid, property p) {
    return c_.call("update_node", name, nid, p).get<int32_t>();
  }

  uint64_t create_edge(std::string name, std::string nid, edge_info ei) {
    return c_.call("create_edge", name, nid, ei).get<uint64_t>();
  }

  int32_t update_edge(std::string name, std::string nid, uint64_t eid, edge_info ei) {
    return c_.call("update_edge", name, nid, eid, ei).get<int32_t>();
  }

  int32_t remove_edge(std::string name, std::string nid, uint64_t e) {
    return c_.call("remove_edge", name, nid, e).get<int32_t>();
  }

  double centrality(std::string name, std::string nid, int32_t ct, preset_query q) {
    return c_.call("centrality", name, nid, ct, q).get<double>();
  }

  bool add_centrality_query(std::string name, preset_query q) {
    return c_.call("add_centrality_query", name, q).get<bool>();
  }

  bool add_shortest_path_query(std::string name, preset_query q) {
    return c_.call("add_shortest_path_query", name, q).get<bool>();
  }

  bool remove_centrality_query(std::string name, preset_query q) {
    return c_.call("remove_centrality_query", name, q).get<bool>();
  }

  bool remove_shortest_path_query(std::string name, preset_query q) {
    return c_.call("remove_shortest_path_query", name, q).get<bool>();
  }

  std::vector<std::string > shortest_path(std::string name, shortest_path_req r) {
    return c_.call("shortest_path", name, r).get<std::vector<std::string > >();
  }

  int32_t update_index(std::string name) {
    return c_.call("update_index", name).get<int32_t>();
  }

  int32_t clear(std::string name) {
    return c_.call("clear", name).get<int32_t>();
  }

  node_info get_node(std::string name, std::string nid) {
    return c_.call("get_node", name, nid).get<node_info>();
  }

  edge_info get_edge(std::string name, std::string nid, uint64_t e) {
    return c_.call("get_edge", name, nid, e).get<edge_info>();
  }

  bool save(std::string name, std::string arg1) {
    return c_.call("save", name, arg1).get<bool>();
  }

  bool load(std::string name, std::string arg1) {
    return c_.call("load", name, arg1).get<bool>();
  }

  std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
    return c_.call("get_status", name).get<std::map<std::string, std::map<std::string, std::string > > >();
  }

  int32_t create_node_here(std::string name, std::string nid) {
    return c_.call("create_node_here", name, nid).get<int32_t>();
  }

  int32_t remove_global_node(std::string name, std::string nid) {
    return c_.call("remove_global_node", name, nid).get<int32_t>();
  }

  int32_t create_edge_here(std::string name, uint64_t eid, edge_info ei) {
    return c_.call("create_edge_here", name, eid, ei).get<int32_t>();
  }

private:
  msgpack::rpc::client c_;
};

} // namespace client

} // namespace jubatus



#endif // GRAPH_CLIENT_HPP_
