// This file is auto-generated from graph.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "../common/exception.hpp"
#include "../framework/aggregators.hpp"
#include "../framework/keeper.hpp"
#include "graph_types.hpp"

int main(int args, char* argv[]) {
  try {
    jubatus::framework::keeper k(
        jubatus::framework::keeper_argv(args, argv, "graph"));
    k.register_async_random<std::string>("get_config");
    k.register_async_random<std::string>("create_node");
    k.register_async_cht<2, bool>("remove_node", pfi::lang::function<bool(bool,
         bool)>(&pass<bool>));
    k.register_async_cht<2, bool, std::map<std::string, std::string> >(
        "update_node", pfi::lang::function<bool(bool, bool)>(&all_and));
    k.register_async_cht<1, uint64_t, edge>("create_edge",
         pfi::lang::function<uint64_t(uint64_t, uint64_t)>(&pass<uint64_t>));
    k.register_async_cht<2, bool, uint64_t, edge>("update_edge",
         pfi::lang::function<bool(bool, bool)>(&all_and));
    k.register_async_cht<2, bool, uint64_t>("remove_edge",
         pfi::lang::function<bool(bool, bool)>(&all_and));
    k.register_async_random<double, std::string, int32_t, preset_query>(
        "get_centrality");
    k.register_async_broadcast<bool, preset_query>("add_centrality_query",
         pfi::lang::function<bool(bool, bool)>(&all_and));
    k.register_async_broadcast<bool, preset_query>("add_shortest_path_query",
         pfi::lang::function<bool(bool, bool)>(&all_and));
    k.register_async_broadcast<bool, preset_query>("remove_centrality_query",
         pfi::lang::function<bool(bool, bool)>(&all_and));
    k.register_async_broadcast<bool, preset_query>("remove_shortest_path_query",
         pfi::lang::function<bool(bool, bool)>(&all_and));
    k.register_async_random<std::vector<std::string>, shortest_path_query>(
        "get_shortest_path");
    k.register_async_broadcast<bool>("update_index", pfi::lang::function<bool(
        bool, bool)>(&all_and));
    k.register_async_broadcast<bool>("clear", pfi::lang::function<bool(bool,
         bool)>(&all_and));
    k.register_async_cht<2, node>("get_node", pfi::lang::function<node(node,
         node)>(&pass<node>));
    k.register_async_cht<2, edge, uint64_t>("get_edge",
         pfi::lang::function<edge(edge, edge)>(&pass<edge>));
    k.register_async_broadcast<bool, std::string>("save",
         pfi::lang::function<bool(bool, bool)>(&all_and));
    k.register_async_broadcast<bool, std::string>("load",
         pfi::lang::function<bool(bool, bool)>(&all_and));
    k.register_async_broadcast<std::map<std::string, std::map<std::string,
         std::string> > >("get_status",
         pfi::lang::function<std::map<std::string, std::map<std::string,
         std::string> >(std::map<std::string, std::map<std::string,
         std::string> >, std::map<std::string, std::map<std::string,
         std::string> >)>(&merge<std::string, std::map<std::string,
         std::string> >));
    return k.run();
  } catch (const jubatus::exception::jubatus_exception& e) {
    LOG(FATAL) << e.diagnostic_information(true);
    return -1;
  }
}
