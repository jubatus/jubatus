
// This file is auto-generated from graph.idl
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

    rpc_server::add<std::string(std::string) >("create_node", pfi::lang::bind(&Impl::create_node, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<int32_t(std::string, std::string) >("remove_node", pfi::lang::bind(&Impl::remove_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<int32_t(std::string, std::string, property) >("update_node", pfi::lang::bind(&Impl::update_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<int32_t(std::string, std::string, edge_info) >("create_edge", pfi::lang::bind(&Impl::create_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<int32_t(std::string, std::string, edge_id_t, edge_info) >("update_edge", pfi::lang::bind(&Impl::update_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4));
    rpc_server::add<int32_t(std::string, std::string, edge_id_t) >("remove_edge", pfi::lang::bind(&Impl::remove_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<double(std::string, std::string, centrality_type, preset_query) >("centrality", pfi::lang::bind(&Impl::centrality, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4));
    rpc_server::add<bool(std::string, preset_query) >("add_cenrality_query", pfi::lang::bind(&Impl::add_cenrality_query, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, preset_query) >("add_shortest_path_query", pfi::lang::bind(&Impl::add_shortest_path_query, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, preset_query) >("remove_cenrality_query", pfi::lang::bind(&Impl::remove_cenrality_query, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, preset_query) >("remove_shortest_path_query", pfi::lang::bind(&Impl::remove_shortest_path_query, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::vector<node_id >(std::string, shortest_path_req) >("shortest_path", pfi::lang::bind(&Impl::shortest_path, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<int32_t(std::string) >("update_index", pfi::lang::bind(&Impl::update_index, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<int32_t(std::string) >("clear", pfi::lang::bind(&Impl::clear, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<node_info(std::string, std::string) >("get_node", pfi::lang::bind(&Impl::get_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<edge_info(std::string, std::string, edge_id_t) >("get_edge", pfi::lang::bind(&Impl::get_edge, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<bool(std::string, std::string) >("save", pfi::lang::bind(&Impl::save, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string) >("load", pfi::lang::bind(&Impl::load, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string > >(std::string) >("get_status", pfi::lang::bind(&Impl::get_status, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<int32_t(std::string, std::string) >("create_global_node", pfi::lang::bind(&Impl::create_global_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<int32_t(std::string, std::string) >("remove_global_node", pfi::lang::bind(&Impl::remove_global_node, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
  }
};

} // namespace server

} // namespace jubatus



#endif // GRAPH_SERVER_HPP_
