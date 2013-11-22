// This file is auto-generated from graph.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_GRAPH_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_GRAPH_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/bind.h"

#include "../../server/common/mprpc/rpc_server.hpp"
#include "graph_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class graph : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit graph(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", jubatus::util::lang::bind(
        &Impl::get_config, impl, jubatus::util::lang::_1));
    rpc_server::add<std::string(std::string)>("create_node", jubatus::util::lang::bind(
        &Impl::create_node, impl, jubatus::util::lang::_1));
    rpc_server::add<bool(std::string, std::string)>("remove_node",
        jubatus::util::lang::bind(&Impl::remove_node, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string, std::map<std::string,
        std::string>)>("update_node", jubatus::util::lang::bind(&Impl::update_node, impl,
        jubatus::util::lang::_1, jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<uint64_t(std::string, std::string, edge)>("create_edge",
        jubatus::util::lang::bind(&Impl::create_edge, impl, jubatus::util::lang::_1, jubatus::util::lang::_2,
        jubatus::util::lang::_3));
    rpc_server::add<bool(std::string, std::string, uint64_t, edge)>(
        "update_edge", jubatus::util::lang::bind(&Impl::update_edge, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2, jubatus::util::lang::_3, jubatus::util::lang::_4));
    rpc_server::add<bool(std::string, std::string, uint64_t)>("remove_edge",
        jubatus::util::lang::bind(&Impl::remove_edge, impl, jubatus::util::lang::_1, jubatus::util::lang::_2,
        jubatus::util::lang::_3));
    rpc_server::add<double(std::string, std::string, int32_t,
        jubatus::core::graph::preset_query)>("get_centrality", jubatus::util::lang::bind(
        &Impl::get_centrality, impl, jubatus::util::lang::_1, jubatus::util::lang::_2,
        jubatus::util::lang::_3, jubatus::util::lang::_4));
    rpc_server::add<bool(std::string, jubatus::core::graph::preset_query)>(
        "add_centrality_query", jubatus::util::lang::bind(&Impl::add_centrality_query,
        impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, jubatus::core::graph::preset_query)>(
        "add_shortest_path_query", jubatus::util::lang::bind(
        &Impl::add_shortest_path_query, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, jubatus::core::graph::preset_query)>(
        "remove_centrality_query", jubatus::util::lang::bind(
        &Impl::remove_centrality_query, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, jubatus::core::graph::preset_query)>(
        "remove_shortest_path_query", jubatus::util::lang::bind(
        &Impl::remove_shortest_path_query, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<std::vector<std::string>(std::string, shortest_path_query)>(
        "get_shortest_path", jubatus::util::lang::bind(&Impl::get_shortest_path, impl,
        jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string)>("update_index", jubatus::util::lang::bind(
        &Impl::update_index, impl, jubatus::util::lang::_1));
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(&Impl::clear,
        impl, jubatus::util::lang::_1));
    rpc_server::add<jubatus::core::graph::node_info(std::string, std::string)>(
        "get_node", jubatus::util::lang::bind(&Impl::get_node, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2));
    rpc_server::add<edge(std::string, std::string, uint64_t)>("get_edge",
        jubatus::util::lang::bind(&Impl::get_edge, impl, jubatus::util::lang::_1, jubatus::util::lang::_2,
        jubatus::util::lang::_3));
    rpc_server::add<bool(std::string, std::string)>("save", jubatus::util::lang::bind(
        &Impl::save, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load", jubatus::util::lang::bind(
        &Impl::load, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", jubatus::util::lang::bind(&Impl::get_status, impl,
        jubatus::util::lang::_1));
    rpc_server::add<bool(std::string, std::string)>("create_node_here",
        jubatus::util::lang::bind(&Impl::create_node_here, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("remove_global_node",
        jubatus::util::lang::bind(&Impl::remove_global_node, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, uint64_t, edge)>("create_edge_here",
        jubatus::util::lang::bind(&Impl::create_edge_here, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2, jubatus::util::lang::_3));
  }
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_GRAPH_SERVER_HPP_
