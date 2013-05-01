// This file is auto-generated from graph.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_GRAPH_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_GRAPH_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/bind.h>

#include "../common/mprpc/rpc_server.hpp"
#include "graph_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class graph : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit graph(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", pfi::lang::bind(
        &Impl::get_config, impl, pfi::lang::_1));
    rpc_server::add<std::string(std::string)>("create_node", pfi::lang::bind(
        &Impl::create_node, impl, pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string)>("remove_node",
         pfi::lang::bind(&Impl::remove_node, impl, pfi::lang::_1,
         pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string, std::map<std::string,
         std::string>)>("update_node", pfi::lang::bind(&Impl::update_node, impl,
         pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<uint64_t(std::string, std::string, edge)>("create_edge",
         pfi::lang::bind(&Impl::create_edge, impl, pfi::lang::_1, pfi::lang::_2,
         pfi::lang::_3));
    rpc_server::add<bool(std::string, std::string, uint64_t, edge)>(
        "update_edge", pfi::lang::bind(&Impl::update_edge, impl, pfi::lang::_1,
         pfi::lang::_2, pfi::lang::_3, pfi::lang::_4));
    rpc_server::add<bool(std::string, std::string, uint64_t)>("remove_edge",
         pfi::lang::bind(&Impl::remove_edge, impl, pfi::lang::_1, pfi::lang::_2,
         pfi::lang::_3));
    rpc_server::add<double(std::string, std::string, int32_t, preset_query)>(
        "get_centrality", pfi::lang::bind(&Impl::get_centrality, impl,
         pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4));
    rpc_server::add<bool(std::string, preset_query)>("add_centrality_query",
         pfi::lang::bind(&Impl::add_centrality_query, impl, pfi::lang::_1,
         pfi::lang::_2));
    rpc_server::add<bool(std::string, preset_query)>("add_shortest_path_query",
         pfi::lang::bind(&Impl::add_shortest_path_query, impl, pfi::lang::_1,
         pfi::lang::_2));
    rpc_server::add<bool(std::string, preset_query)>("remove_centrality_query",
         pfi::lang::bind(&Impl::remove_centrality_query, impl, pfi::lang::_1,
         pfi::lang::_2));
    rpc_server::add<bool(std::string, preset_query)>(
        "remove_shortest_path_query", pfi::lang::bind(
        &Impl::remove_shortest_path_query, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::vector<std::string>(std::string, shortest_path_query)>(
        "get_shortest_path", pfi::lang::bind(&Impl::get_shortest_path, impl,
         pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string)>("update_index", pfi::lang::bind(
        &Impl::update_index, impl, pfi::lang::_1));
    rpc_server::add<bool(std::string)>("clear", pfi::lang::bind(&Impl::clear,
         impl, pfi::lang::_1));
    rpc_server::add<node(std::string, std::string)>("get_node", pfi::lang::bind(
        &Impl::get_node, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<edge(std::string, std::string, uint64_t)>("get_edge",
         pfi::lang::bind(&Impl::get_edge, impl, pfi::lang::_1, pfi::lang::_2,
         pfi::lang::_3));
    rpc_server::add<bool(std::string, std::string)>("save", pfi::lang::bind(
        &Impl::save, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load", pfi::lang::bind(
        &Impl::load, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", pfi::lang::bind(&Impl::get_status, impl,
         pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string)>("create_node_here",
         pfi::lang::bind(&Impl::create_node_here, impl, pfi::lang::_1,
         pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("remove_global_node",
         pfi::lang::bind(&Impl::remove_global_node, impl, pfi::lang::_1,
         pfi::lang::_2));
    rpc_server::add<bool(std::string, uint64_t, edge)>("create_edge_here",
         pfi::lang::bind(&Impl::create_edge_here, impl, pfi::lang::_1,
         pfi::lang::_2, pfi::lang::_3));
  }
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_GRAPH_SERVER_HPP_
