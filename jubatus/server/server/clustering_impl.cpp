// This file is auto-generated from clustering.idl(0.6.4-33-gcc8d7ca) with jenerator version 0.6.4-107-ge23137f/undefined
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "clustering_serv.hpp"

namespace jubatus {
namespace server {

class clustering_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit clustering_impl(const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<clustering_serv>(a,
        false)) {

    rpc_server::add("push", jubatus::util::lang::bind(&clustering_impl::push,
        this, jubatus::util::lang::_1));
    rpc_server::add("get_revision", jubatus::util::lang::bind(
        &clustering_impl::get_revision, this, jubatus::util::lang::_1));
    rpc_server::add("get_core_members", jubatus::util::lang::bind(
        &clustering_impl::get_core_members, this, jubatus::util::lang::_1));
    rpc_server::add("get_k_center", jubatus::util::lang::bind(
        &clustering_impl::get_k_center, this, jubatus::util::lang::_1));
    rpc_server::add("get_nearest_center", jubatus::util::lang::bind(
        &clustering_impl::get_nearest_center, this, jubatus::util::lang::_1));
    rpc_server::add("get_nearest_members", jubatus::util::lang::bind(
        &clustering_impl::get_nearest_members, this, jubatus::util::lang::_1));
    rpc_server::add("clear", jubatus::util::lang::bind(&clustering_impl::clear,
        this, jubatus::util::lang::_1));

    rpc_server::add("get_config", jubatus::util::lang::bind(
        &clustering_impl::get_config, this, jubatus::util::lang::_1));
    rpc_server::add("save", jubatus::util::lang::bind(&clustering_impl::save,
        this, jubatus::util::lang::_1));
    rpc_server::add("load", jubatus::util::lang::bind(&clustering_impl::load,
        this, jubatus::util::lang::_1));
    rpc_server::add("get_status", jubatus::util::lang::bind(
        &clustering_impl::get_status, this, jubatus::util::lang::_1));
  }

  void push(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        std::vector<jubatus::core::fv_converter::datum> > params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->push(params.get<1>());
    req.result(retval);
  }

  void get_revision(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    uint32_t retval = get_p()->get_revision();
    req.result(retval);
  }

  void get_core_members(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<std::vector<std::pair<double,
        jubatus::core::fv_converter::datum> > > retval = get_p(
        )->get_core_members();
    req.result(retval);
  }

  void get_k_center(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<jubatus::core::fv_converter::datum> retval = get_p(
        )->get_k_center();
    req.result(retval);
  }

  void get_nearest_center(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        jubatus::core::fv_converter::datum> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    jubatus::core::fv_converter::datum retval = get_p()->get_nearest_center(
        params.get<1>());
    req.result(retval);
  }

  void get_nearest_members(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        jubatus::core::fv_converter::datum> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<std::pair<double,
        jubatus::core::fv_converter::datum> > retval = get_p(
        )->get_nearest_members(params.get<1>());
    req.result(retval);
  }

  void clear(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->clear();
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
  jubatus::util::lang::shared_ptr<clustering_serv> get_p() { return p_->server(
      ); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<clustering_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::clustering_impl>
      (argc, argv, "clustering");
}
