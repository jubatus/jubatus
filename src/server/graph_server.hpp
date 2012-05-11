
// This file is auto-generated from src/server/graph.idl
// *** DO NOT EDIT ***

#ifndef GRAPH_SERVER_HPP_
#define GRAPH_SERVER_HPP_


#include "graph_types.hpp"
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/bind.h>


namespace jubatus {

namespace server {

template <class Impl>
class graph : public pfi::network::mprpc::rpc_server {
public:
  graph(double timeout_sec): rpc_server(timeout_sec) {

    rpc_server::add<int32_t(std::string, node_id, int32_t) >("create_node", pfi::lang::bind(&Impl::create_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<int32_t(std::string, node_id) >("create_global_node", pfi::lang::bind(&Impl::create_global_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<int32_t(std::string, node_id) >("remove_global_node", pfi::lang::bind(&Impl::remove_global_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<int32_t(std::string, node_id, property) >("update_node", pfi::lang::bind(&Impl::update_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<int32_t(std::string, node_id, int32_t) >("remove_node", pfi::lang::bind(&Impl::remove_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<int32_t(std::string, node_id, edge_req) >("create_edge", pfi::lang::bind(&Impl::create_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<int32_t(std::string, node_id, edge_req) >("update_edge", pfi::lang::bind(&Impl::update_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<int32_t(std::string, node_id, edge_id_t) >("remove_edge", pfi::lang::bind(&Impl::remove_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<double(std::string, std::pair<node_id, centrality_type >) >("centrality", pfi::lang::bind(&Impl::centrality, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::vector<node_id >(std::string, shortest_path_req) >("shortest_path", pfi::lang::bind(&Impl::shortest_path, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<int32_t(std::string, int32_t) >("update_index", pfi::lang::bind(&Impl::update_index, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<int32_t(std::string, int32_t) >("clear", pfi::lang::bind(&Impl::clear, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<node_info(std::string, node_id, int32_t) >("get_node", pfi::lang::bind(&Impl::get_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<edge_info(std::string, node_id, edge_id_t) >("get_edge", pfi::lang::bind(&Impl::get_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<int32_t(std::string, std::string) >("save", pfi::lang::bind(&Impl::save, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<int32_t(std::string, std::string) >("load", pfi::lang::bind(&Impl::load, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string > >(std::string, int32_t) >("get_status", pfi::lang::bind(&Impl::get_status, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
  }
};

} // namespace server

} // namespace jubatus



#endif // GRAPH_SERVER_HPP_
