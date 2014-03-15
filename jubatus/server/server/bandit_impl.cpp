// This file is auto-generated from bandit.idl(0.6.4-37-g8b6a586) with jenerator version 0.6.4-107-ge23137f/undefined
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "bandit_serv.hpp"

namespace jubatus {
namespace server {

class bandit_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit bandit_impl(const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<bandit_serv>(a, true)) {

    rpc_server::add("register_arm", jubatus::util::lang::bind(
        &bandit_impl::register_arm, this, jubatus::util::lang::_1));
    rpc_server::add("delete_arm", jubatus::util::lang::bind(
        &bandit_impl::delete_arm, this, jubatus::util::lang::_1));
    rpc_server::add("select_arm", jubatus::util::lang::bind(
        &bandit_impl::select_arm, this, jubatus::util::lang::_1));
    rpc_server::add("register_reward", jubatus::util::lang::bind(
        &bandit_impl::register_reward, this, jubatus::util::lang::_1));
    rpc_server::add("get_arm_info", jubatus::util::lang::bind(
        &bandit_impl::get_arm_info, this, jubatus::util::lang::_1));
    rpc_server::add("reset", jubatus::util::lang::bind(&bandit_impl::reset,
        this, jubatus::util::lang::_1));
    rpc_server::add("clear", jubatus::util::lang::bind(&bandit_impl::clear,
        this, jubatus::util::lang::_1));

    rpc_server::add("get_config", jubatus::util::lang::bind(
        &bandit_impl::get_config, this, jubatus::util::lang::_1));
    rpc_server::add("save", jubatus::util::lang::bind(&bandit_impl::save, this,
        jubatus::util::lang::_1));
    rpc_server::add("load", jubatus::util::lang::bind(&bandit_impl::load, this,
        jubatus::util::lang::_1));
    rpc_server::add("get_status", jubatus::util::lang::bind(
        &bandit_impl::get_status, this, jubatus::util::lang::_1));
  }

  void register_arm(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->register_arm(params.get<1>());
    req.result(retval);
  }

  void delete_arm(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->delete_arm(params.get<1>());
    req.result(retval);
  }

  void select_arm(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    std::string retval = get_p()->select_arm(params.get<1>());
    req.result(retval);
  }

  void register_reward(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string, std::string, double> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->register_reward(params.get<1>(), params.get<2>(),
        params.get<3>());
    req.result(retval);
  }

  void get_arm_info(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::map<std::string, arm_info> retval = get_p()->get_arm_info(
        params.get<1>());
    req.result(retval);
  }

  void reset(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->reset(params.get<1>());
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
  jubatus::util::lang::shared_ptr<bandit_serv> get_p() { return p_->server(); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<bandit_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::bandit_impl>
      (argc, argv, "bandit");
}
