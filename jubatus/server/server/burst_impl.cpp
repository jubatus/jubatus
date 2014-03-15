// This file is auto-generated from burst.idl(0.6.4-96-g66ed74d) with jenerator version 0.6.4-107-ge23137f/undefined
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "burst_serv.hpp"

namespace jubatus {
namespace server {

class burst_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit burst_impl(const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<burst_serv>(a, true)) {

    rpc_server::add("add_documents", jubatus::util::lang::bind(
        &burst_impl::add_documents, this, jubatus::util::lang::_1));
    rpc_server::add("get_result", jubatus::util::lang::bind(
        &burst_impl::get_result, this, jubatus::util::lang::_1));
    rpc_server::add("get_result_at", jubatus::util::lang::bind(
        &burst_impl::get_result_at, this, jubatus::util::lang::_1));
    rpc_server::add("get_all_bursted_results", jubatus::util::lang::bind(
        &burst_impl::get_all_bursted_results, this, jubatus::util::lang::_1));
    rpc_server::add("get_all_bursted_results_at", jubatus::util::lang::bind(
        &burst_impl::get_all_bursted_results_at, this,
        jubatus::util::lang::_1));
    rpc_server::add("get_all_keywords", jubatus::util::lang::bind(
        &burst_impl::get_all_keywords, this, jubatus::util::lang::_1));
    rpc_server::add("add_keyword", jubatus::util::lang::bind(
        &burst_impl::add_keyword, this, jubatus::util::lang::_1));
    rpc_server::add("remove_keyword", jubatus::util::lang::bind(
        &burst_impl::remove_keyword, this, jubatus::util::lang::_1));
    rpc_server::add("remove_all_keywords", jubatus::util::lang::bind(
        &burst_impl::remove_all_keywords, this, jubatus::util::lang::_1));
    rpc_server::add("clear", jubatus::util::lang::bind(&burst_impl::clear, this,
        jubatus::util::lang::_1));

    rpc_server::add("get_config", jubatus::util::lang::bind(
        &burst_impl::get_config, this, jubatus::util::lang::_1));
    rpc_server::add("save", jubatus::util::lang::bind(&burst_impl::save, this,
        jubatus::util::lang::_1));
    rpc_server::add("load", jubatus::util::lang::bind(&burst_impl::load, this,
        jubatus::util::lang::_1));
    rpc_server::add("get_status", jubatus::util::lang::bind(
        &burst_impl::get_status, this, jubatus::util::lang::_1));
  }

  void add_documents(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::vector<document> > params;
    req.params().convert(&params);
    JWLOCK_(p_);
    int32_t retval = get_p()->add_documents(params.get<1>());
    req.result(retval);
  }

  void get_result(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    window retval = get_p()->get_result(params.get<1>());
    req.result(retval);
  }

  void get_result_at(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string, double> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    window retval = get_p()->get_result_at(params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void get_all_bursted_results(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::map<std::string, window> retval = get_p()->get_all_bursted_results();
    req.result(retval);
  }

  void get_all_bursted_results_at(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, double> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::map<std::string, window> retval = get_p()->get_all_bursted_results_at(
        params.get<1>());
    req.result(retval);
  }

  void get_all_keywords(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<keyword_with_params> retval = get_p()->get_all_keywords();
    req.result(retval);
  }

  void add_keyword(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, keyword_with_params> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->add_keyword(params.get<1>());
    req.result(retval);
  }

  void remove_keyword(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->remove_keyword(params.get<1>());
    req.result(retval);
  }

  void remove_all_keywords(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->remove_all_keywords();
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
  jubatus::util::lang::shared_ptr<burst_serv> get_p() { return p_->server(); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<burst_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::burst_impl>
      (argc, argv, "burst");
}
