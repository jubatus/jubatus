
#include "graph_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class graph : public pfi::network::mprpc::rpc_client {
public:
  graph(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    std::string create_node(std::string name) {
      return call<std::string(std::string)>("create_node")(name);
    }

    int32_t remove_node(std::string name, std::string nid) {
      return call<int32_t(std::string, std::string)>("remove_node")(name, nid);
    }

    int32_t update_node(std::string name, std::string nid, property p) {
      return call<int32_t(std::string, std::string, property)>("update_node")(name, nid, p);
    }

    uint64_t create_edge(std::string name, std::string nid, edge_info ei) {
      return call<uint64_t(std::string, std::string, edge_info)>("create_edge")(name, nid, ei);
    }

    int32_t update_edge(std::string name, std::string nid, uint64_t eid, edge_info ei) {
      return call<int32_t(std::string, std::string, uint64_t, edge_info)>("update_edge")(name, nid, eid, ei);
    }

    int32_t remove_edge(std::string name, std::string nid, uint64_t e) {
      return call<int32_t(std::string, std::string, uint64_t)>("remove_edge")(name, nid, e);
    }

    double centrality(std::string name, std::string nid, int32_t ct, preset_query q) {
      return call<double(std::string, std::string, int32_t, preset_query)>("centrality")(name, nid, ct, q);
    }

    bool add_centrality_query(std::string name, preset_query q) {
      return call<bool(std::string, preset_query)>("add_centrality_query")(name, q);
    }

    bool add_shortest_path_query(std::string name, preset_query q) {
      return call<bool(std::string, preset_query)>("add_shortest_path_query")(name, q);
    }

    bool remove_centrality_query(std::string name, preset_query q) {
      return call<bool(std::string, preset_query)>("remove_centrality_query")(name, q);
    }

    bool remove_shortest_path_query(std::string name, preset_query q) {
      return call<bool(std::string, preset_query)>("remove_shortest_path_query")(name, q);
    }

    std::vector<std::string > shortest_path(std::string name, shortest_path_req r) {
      return call<std::vector<std::string >(std::string, shortest_path_req)>("shortest_path")(name, r);
    }

    int32_t update_index(std::string name) {
      return call<int32_t(std::string)>("update_index")(name);
    }

    int32_t clear(std::string name) {
      return call<int32_t(std::string)>("clear")(name);
    }

    node_info get_node(std::string name, std::string nid) {
      return call<node_info(std::string, std::string)>("get_node")(name, nid);
    }

    edge_info get_edge(std::string name, std::string nid, uint64_t e) {
      return call<edge_info(std::string, std::string, uint64_t)>("get_edge")(name, nid, e);
    }

    bool save(std::string name, std::string arg1) {
      return call<bool(std::string, std::string)>("save")(name, arg1);
    }

    bool load(std::string name, std::string arg1) {
      return call<bool(std::string, std::string)>("load")(name, arg1);
    }

    std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
      return call<std::map<std::string, std::map<std::string, std::string > >(std::string)>("get_status")(name);
    }

    int32_t create_node_here(std::string name, std::string nid) {
      return call<int32_t(std::string, std::string)>("create_node_here")(name, nid);
    }

    int32_t create_global_node(std::string name, std::string nid) {
      return call<int32_t(std::string, std::string)>("create_global_node")(name, nid);
    }

    int32_t remove_global_node(std::string name, std::string nid) {
      return call<int32_t(std::string, std::string)>("remove_global_node")(name, nid);
    }

    int32_t create_edge_here(std::string name, uint64_t eid, edge_info ei) {
      return call<int32_t(std::string, uint64_t, edge_info)>("create_edge_here")(name, eid, ei);
    }

private:
};

} // namespace client

} // namespace jubatus

