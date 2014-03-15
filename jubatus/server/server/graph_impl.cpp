// This file is auto-generated from graph.idl(0.6.4-33-gcc8d7ca) with jenerator version 0.6.4-107-ge23137f/undefined
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "graph_serv.hpp"

namespace jubatus {
namespace server {

class graph_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit graph_impl(const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<graph_serv>(a, true)) {

    rpc_server::add("create_node", jubatus::util::lang::bind(
        &graph_impl::create_node, this, jubatus::util::lang::_1));
    rpc_server::add("remove_node", jubatus::util::lang::bind(
        &graph_impl::remove_node, this, jubatus::util::lang::_1));
    rpc_server::add("update_node", jubatus::util::lang::bind(
        &graph_impl::update_node, this, jubatus::util::lang::_1));
    rpc_server::add("create_edge", jubatus::util::lang::bind(
        &graph_impl::create_edge, this, jubatus::util::lang::_1));
    rpc_server::add("update_edge", jubatus::util::lang::bind(
        &graph_impl::update_edge, this, jubatus::util::lang::_1));
    rpc_server::add("remove_edge", jubatus::util::lang::bind(
        &graph_impl::remove_edge, this, jubatus::util::lang::_1));
    rpc_server::add("get_centrality", jubatus::util::lang::bind(
        &graph_impl::get_centrality, this, jubatus::util::lang::_1));
    rpc_server::add("add_centrality_query", jubatus::util::lang::bind(
        &graph_impl::add_centrality_query, this, jubatus::util::lang::_1));
    rpc_server::add("add_shortest_path_query", jubatus::util::lang::bind(
        &graph_impl::add_shortest_path_query, this, jubatus::util::lang::_1));
    rpc_server::add("remove_centrality_query", jubatus::util::lang::bind(
        &graph_impl::remove_centrality_query, this, jubatus::util::lang::_1));
    rpc_server::add("remove_shortest_path_query", jubatus::util::lang::bind(
        &graph_impl::remove_shortest_path_query, this,
        jubatus::util::lang::_1));
    rpc_server::add("get_shortest_path", jubatus::util::lang::bind(
        &graph_impl::get_shortest_path, this, jubatus::util::lang::_1));
    rpc_server::add("update_index", jubatus::util::lang::bind(
        &graph_impl::update_index, this, jubatus::util::lang::_1));
    rpc_server::add("clear", jubatus::util::lang::bind(&graph_impl::clear, this,
        jubatus::util::lang::_1));
    rpc_server::add("get_node", jubatus::util::lang::bind(&graph_impl::get_node,
        this, jubatus::util::lang::_1));
    rpc_server::add("get_edge", jubatus::util::lang::bind(&graph_impl::get_edge,
        this, jubatus::util::lang::_1));
    rpc_server::add("create_node_here", jubatus::util::lang::bind(
        &graph_impl::create_node_here, this, jubatus::util::lang::_1));
    rpc_server::add("remove_global_node", jubatus::util::lang::bind(
        &graph_impl::remove_global_node, this, jubatus::util::lang::_1));
    rpc_server::add("create_edge_here", jubatus::util::lang::bind(
        &graph_impl::create_edge_here, this, jubatus::util::lang::_1));

    rpc_server::add("get_config", jubatus::util::lang::bind(
        &graph_impl::get_config, this, jubatus::util::lang::_1));
    rpc_server::add("save", jubatus::util::lang::bind(&graph_impl::save, this,
        jubatus::util::lang::_1));
    rpc_server::add("load", jubatus::util::lang::bind(&graph_impl::load, this,
        jubatus::util::lang::_1));
    rpc_server::add("get_status", jubatus::util::lang::bind(
        &graph_impl::get_status, this, jubatus::util::lang::_1));
  }

  void create_node(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    NOLOCK_(p_);
    std::string retval = get_p()->create_node();
    req.result(retval);
  }

  void remove_node(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    NOLOCK_(p_);
    bool retval = get_p()->remove_node(params.get<1>());
    req.result(retval);
  }

  void update_node(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string, std::map<std::string,
        std::string> > params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->update_node(params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void create_edge(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string, edge> params;
    req.params().convert(&params);
    NOLOCK_(p_);
    uint64_t retval = get_p()->create_edge(params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void update_edge(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string, uint64_t, edge> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->update_edge(params.get<1>(), params.get<2>(),
        params.get<3>());
    req.result(retval);
  }

  void remove_edge(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string, uint64_t> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->remove_edge(params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void get_centrality(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string, int32_t,
        jubatus::core::graph::preset_query> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    double retval = get_p()->get_centrality(params.get<1>(), params.get<2>(),
        params.get<3>());
    req.result(retval);
  }

  void add_centrality_query(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        jubatus::core::graph::preset_query> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->add_centrality_query(params.get<1>());
    req.result(retval);
  }

  void add_shortest_path_query(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        jubatus::core::graph::preset_query> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->add_shortest_path_query(params.get<1>());
    req.result(retval);
  }

  void remove_centrality_query(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        jubatus::core::graph::preset_query> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->remove_centrality_query(params.get<1>());
    req.result(retval);
  }

  void remove_shortest_path_query(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        jubatus::core::graph::preset_query> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->remove_shortest_path_query(params.get<1>());
    req.result(retval);
  }

  void get_shortest_path(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, shortest_path_query> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<std::string> retval = get_p()->get_shortest_path(params.get<1>(
        ));
    req.result(retval);
  }

  void update_index(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->update_index();
    req.result(retval);
  }

  void clear(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->clear();
    req.result(retval);
  }

  void get_node(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    jubatus::core::graph::node_info retval = get_p()->get_node(params.get<1>());
    req.result(retval);
  }

  void get_edge(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string, uint64_t> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    edge retval = get_p()->get_edge(params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void create_node_here(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->create_node_here(params.get<1>());
    req.result(retval);
  }

  void remove_global_node(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->remove_global_node(params.get<1>());
    req.result(retval);
  }

  void create_edge_here(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, uint64_t, edge> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->create_edge_here(params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void get_config(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::string retval = get_p()->get_config();
    req.result(retval);
  }

  void save(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    bool retval = get_p()->save(params.get<1>());
    req.result(retval);
  }

  void load(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->load(params.get<1>());
    req.result(retval);
  }

  void get_status(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::map<std::string, std::map<std::string,
        std::string> > retval = p_->get_status();
    req.result(retval);
  }

  int run() { return p_->start(*this); }
  jubatus::util::lang::shared_ptr<graph_serv> get_p() { return p_->server(); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<graph_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::graph_impl>
      (argc, argv, "graph");
}
