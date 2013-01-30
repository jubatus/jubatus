
// This file is auto-generated from graph.idl
// *** DO NOT EDIT ***

#ifndef GRAPH_CLIENT_HPP_
#define GRAPH_CLIENT_HPP_


#include "graph_types.hpp"
#include <jubatus/msgpack/rpc/client.h>


namespace jubatus {

namespace graph {

namespace client {

class graph {
public:
  graph(const std::string &host, uint64_t port, double timeout_sec)
    : c_(host, port) {
    c_.set_timeout( timeout_sec );
  }

    std::string get_config(std::string name) {
      return c_.call("get_config", name).get<std::string >();
    }

    std::string create_node(std::string name) {
      return c_.call("create_node", name).get<std::string >();
    }

    bool remove_node(std::string name, std::string node_id) {
      return c_.call("remove_node", name, node_id).get<bool >();
    }

    bool update_node(std::string name, std::string node_id, std::map<std::string, std::string > property) {
      return c_.call("update_node", name, node_id, property).get<bool >();
    }

    uint64_t create_edge(std::string name, std::string node_id, edge e) {
      return c_.call("create_edge", name, node_id, e).get<uint64_t >();
    }

    bool update_edge(std::string name, std::string node_id, uint64_t edge_id, edge e) {
      return c_.call("update_edge", name, node_id, edge_id, e).get<bool >();
    }

    bool remove_edge(std::string name, std::string node_id, uint64_t edge_id) {
      return c_.call("remove_edge", name, node_id, edge_id).get<bool >();
    }

    double get_centrality(std::string name, std::string node_id, int32_t centrality_type, preset_query query) {
      return c_.call("get_centrality", name, node_id, centrality_type, query).get<double >();
    }

    bool add_centrality_query(std::string name, preset_query query) {
      return c_.call("add_centrality_query", name, query).get<bool >();
    }

    bool add_shortest_path_query(std::string name, preset_query query) {
      return c_.call("add_shortest_path_query", name, query).get<bool >();
    }

    bool remove_centrality_query(std::string name, preset_query query) {
      return c_.call("remove_centrality_query", name, query).get<bool >();
    }

    bool remove_shortest_path_query(std::string name, preset_query query) {
      return c_.call("remove_shortest_path_query", name, query).get<bool >();
    }

    std::vector<std::string > get_shortest_path(std::string name, shortest_path_query query) {
      return c_.call("get_shortest_path", name, query).get<std::vector<std::string > >();
    }

    bool update_index(std::string name) {
      return c_.call("update_index", name).get<bool >();
    }

    bool clear(std::string name) {
      return c_.call("clear", name).get<bool >();
    }

    node get_node(std::string name, std::string node_id) {
      return c_.call("get_node", name, node_id).get<node >();
    }

    edge get_edge(std::string name, std::string node_id, uint64_t edge_id) {
      return c_.call("get_edge", name, node_id, edge_id).get<edge >();
    }

    bool save(std::string name, std::string id) {
      return c_.call("save", name, id).get<bool >();
    }

    bool load(std::string name, std::string id) {
      return c_.call("load", name, id).get<bool >();
    }

    std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
      return c_.call("get_status", name).get<std::map<std::string, std::map<std::string, std::string > > >();
    }

    bool create_node_here(std::string name, std::string node_id) {
      return c_.call("create_node_here", name, node_id).get<bool >();
    }

    bool remove_global_node(std::string name, std::string node_id) {
      return c_.call("remove_global_node", name, node_id).get<bool >();
    }

    bool create_edge_here(std::string name, uint64_t edge_id, edge e) {
      return c_.call("create_edge_here", name, edge_id, e).get<bool >();
    }

private:
  msgpack::rpc::client c_;
};

} // namespace client

} // namespace graph

} // namespace jubatus



#endif // GRAPH_CLIENT_HPP_
