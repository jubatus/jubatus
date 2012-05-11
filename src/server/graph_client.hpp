
#include "graph_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class graph : public pfi::network::mprpc::rpc_client {
public:
  graph(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    int32_t create_node(std::string arg0, node_id arg1, int32_t arg2) {
      return call<int32_t(std::string, node_id, int32_t)>("create_node")(arg0, arg1, arg2);
    }

    int32_t create_global_node(std::string arg0, node_id arg1) {
      return call<int32_t(std::string, node_id)>("create_global_node")(arg0, arg1);
    }

    int32_t remove_global_node(std::string arg0, node_id arg1) {
      return call<int32_t(std::string, node_id)>("remove_global_node")(arg0, arg1);
    }

    int32_t update_node(std::string arg0, node_id arg1, property arg2) {
      return call<int32_t(std::string, node_id, property)>("update_node")(arg0, arg1, arg2);
    }

    int32_t remove_node(std::string arg0, node_id arg1, int32_t arg2) {
      return call<int32_t(std::string, node_id, int32_t)>("remove_node")(arg0, arg1, arg2);
    }

    int32_t create_edge(std::string arg0, node_id arg1, edge_req arg2) {
      return call<int32_t(std::string, node_id, edge_req)>("create_edge")(arg0, arg1, arg2);
    }

    int32_t update_edge(std::string arg0, node_id arg1, edge_req arg2) {
      return call<int32_t(std::string, node_id, edge_req)>("update_edge")(arg0, arg1, arg2);
    }

    int32_t remove_edge(std::string arg0, node_id arg1, edge_id_t arg2) {
      return call<int32_t(std::string, node_id, edge_id_t)>("remove_edge")(arg0, arg1, arg2);
    }

    double centrality(std::string arg0, std::pair<node_id, centrality_type > arg1) {
      return call<double(std::string, std::pair<node_id, centrality_type >)>("centrality")(arg0, arg1);
    }

    std::vector<node_id > shortest_path(std::string arg0, shortest_path_req arg1) {
      return call<std::vector<node_id >(std::string, shortest_path_req)>("shortest_path")(arg0, arg1);
    }

    int32_t update_index(std::string arg0, int32_t arg1) {
      return call<int32_t(std::string, int32_t)>("update_index")(arg0, arg1);
    }

    int32_t clear(std::string arg0, int32_t arg1) {
      return call<int32_t(std::string, int32_t)>("clear")(arg0, arg1);
    }

    node_info get_node(std::string arg0, node_id arg1, int32_t arg2) {
      return call<node_info(std::string, node_id, int32_t)>("get_node")(arg0, arg1, arg2);
    }

    edge_info get_edge(std::string arg0, node_id arg1, edge_id_t arg2) {
      return call<edge_info(std::string, node_id, edge_id_t)>("get_edge")(arg0, arg1, arg2);
    }

    int32_t save(std::string arg0, std::string arg1) {
      return call<int32_t(std::string, std::string)>("save")(arg0, arg1);
    }

    int32_t load(std::string arg0, std::string arg1) {
      return call<int32_t(std::string, std::string)>("load")(arg0, arg1);
    }

    std::map<std::string, std::map<std::string, std::string > > get_status(std::string arg0, int32_t arg1) {
      return call<std::map<std::string, std::map<std::string, std::string > >(std::string, int32_t)>("get_status")(arg0, arg1);
    }

private:
};

} // namespace client

} // namespace jubatus

