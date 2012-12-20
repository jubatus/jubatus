
// This file is auto-generated from graph.idl
// *** DO NOT EDIT ***

#ifndef GRAPH_SERVER_HPP_
#define GRAPH_SERVER_HPP_


#include "graph_types.hpp"
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/bind.h>
#include "../common/mprpc/rpc_server.hpp"


namespace jubatus {

namespace server {

template <class Impl>
class graph : public jubatus::common::mprpc::rpc_server {
public:
  graph(double timeout_sec): rpc_server(timeout_sec) {

    rpc_server::add<std::string(std::string) >("create_node", pfi::lang::bind(&Impl::create_node, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string) >("remove_node", pfi::lang::bind(&Impl::remove_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string, property) >("update_node", pfi::lang::bind(&Impl::update_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<uint64_t(std::string, std::string, edge_info) >("create_edge", pfi::lang::bind(&Impl::create_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<bool(std::string, std::string, uint64_t, edge_info) >("update_edge", pfi::lang::bind(&Impl::update_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4));
    rpc_server::add<bool(std::string, std::string, uint64_t) >("remove_edge", pfi::lang::bind(&Impl::remove_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<double(std::string, std::string, int32_t, preset_query) >("centrality", pfi::lang::bind(&Impl::centrality, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4));
    rpc_server::add<bool(std::string, preset_query) >("add_centrality_query", pfi::lang::bind(&Impl::add_centrality_query, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, preset_query) >("add_shortest_path_query", pfi::lang::bind(&Impl::add_shortest_path_query, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, preset_query) >("remove_centrality_query", pfi::lang::bind(&Impl::remove_centrality_query, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, preset_query) >("remove_shortest_path_query", pfi::lang::bind(&Impl::remove_shortest_path_query, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::vector<std::string >(std::string, shortest_path_req) >("shortest_path", pfi::lang::bind(&Impl::shortest_path, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string) >("update_index", pfi::lang::bind(&Impl::update_index, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<bool(std::string) >("clear", pfi::lang::bind(&Impl::clear, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<node_info(std::string, std::string) >("get_node", pfi::lang::bind(&Impl::get_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<edge_info(std::string, std::string, uint64_t) >("get_edge", pfi::lang::bind(&Impl::get_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<bool(std::string, std::string) >("save", pfi::lang::bind(&Impl::save, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string) >("load", pfi::lang::bind(&Impl::load, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string > >(std::string) >("get_status", pfi::lang::bind(&Impl::get_status, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string) >("create_node_here", pfi::lang::bind(&Impl::create_node_here, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string) >("remove_global_node", pfi::lang::bind(&Impl::remove_global_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, uint64_t, edge_info) >("create_edge_here", pfi::lang::bind(&Impl::create_edge_here, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
  }
};

} // namespace server

} // namespace jubatus



#endif // GRAPH_SERVER_HPP_
