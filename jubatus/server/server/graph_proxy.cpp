// This file is auto-generated from graph.idl(0.4.3-274-g83cbcee) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "../../server/framework/aggregators.hpp"
#include "../../server/framework/proxy.hpp"
#include "graph_types.hpp"

namespace jubatus {

int run_proxy(int argc, char* argv[]) {
  try {
    jubatus::server::framework::proxy k(
        jubatus::server::framework::proxy_argv(argc, argv, "graph"));
    k.register_async_random<std::string>("create_node");
    k.register_async_cht<2, bool>("remove_node", pfi::lang::function<bool(bool,
        bool)>(&jubatus::server::framework::pass<bool>));
    k.register_async_cht<2, bool, std::map<std::string, std::string> >(
        "update_node", pfi::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_cht<1, uint64_t, edge>("create_edge",
        pfi::lang::function<uint64_t(uint64_t, uint64_t)>(
        &jubatus::server::framework::pass<uint64_t>));
    k.register_async_cht<2, bool, uint64_t, edge>("update_edge",
        pfi::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_cht<2, bool, uint64_t>("remove_edge",
        pfi::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_random<double, std::string, int32_t,
        jubatus::core::graph::preset_query>("get_centrality");
    k.register_async_broadcast<bool, jubatus::core::graph::preset_query>(
        "add_centrality_query", pfi::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool, jubatus::core::graph::preset_query>(
        "add_shortest_path_query", pfi::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool, jubatus::core::graph::preset_query>(
        "remove_centrality_query", pfi::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool, jubatus::core::graph::preset_query>(
        "remove_shortest_path_query", pfi::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_random<std::vector<std::string>, shortest_path_query>(
        "get_shortest_path");
    k.register_async_broadcast<bool>("update_index", pfi::lang::function<bool(
        bool, bool)>(&jubatus::server::framework::all_and));
    k.register_async_broadcast<bool>("clear", pfi::lang::function<bool(bool,
        bool)>(&jubatus::server::framework::all_and));
    k.register_async_cht<2, jubatus::core::graph::node_info>("get_node",
        pfi::lang::function<jubatus::core::graph::node_info(
        jubatus::core::graph::node_info, jubatus::core::graph::node_info)>(
        &jubatus::server::framework::pass<jubatus::core::graph::node_info>));
    k.register_async_cht<2, edge, uint64_t>("get_edge",
        pfi::lang::function<edge(edge, edge)>(
        &jubatus::server::framework::pass<edge>));
    return k.run();
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    LOG(FATAL) << e.diagnostic_information(true);
    return -1;
  }
}

}  // namespace jubatus

int main(int argc, char* argv[]) {
  jubatus::run_proxy(argc, argv);
}
