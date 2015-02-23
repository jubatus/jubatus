// This file is auto-generated from bandit.idl(0.6.4-14-g3b68062) with jenerator version 0.5.4-224-g49229fa/feature/bandit
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

    rpc_server::add<bool(std::string, std::string)>("register_arm",
        jubatus::util::lang::bind(&bandit_impl::register_arm, this,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("delete_arm",
        jubatus::util::lang::bind(&bandit_impl::delete_arm, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::string(std::string, std::string)>("select_arm",
        jubatus::util::lang::bind(&bandit_impl::select_arm, this,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string, std::string, double)>(
        "register_reward", jubatus::util::lang::bind(
        &bandit_impl::register_reward, this, jubatus::util::lang::_2,
        jubatus::util::lang::_3, jubatus::util::lang::_4));
    rpc_server::add<std::map<std::string, arm_info>(std::string, std::string)>(
        "get_arm_info", jubatus::util::lang::bind(&bandit_impl::get_arm_info,
        this, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("reset",
        jubatus::util::lang::bind(&bandit_impl::reset, this,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(
        &bandit_impl::clear, this));

    rpc_server::add<std::string(std::string)>("get_config",
        jubatus::util::lang::bind(&bandit_impl::get_config, this));
    rpc_server::add<bool(std::string, std::string)>("save",
        jubatus::util::lang::bind(&bandit_impl::save, this,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load",
        jubatus::util::lang::bind(&bandit_impl::load, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", jubatus::util::lang::bind(
        &bandit_impl::get_status, this));
  }

  bool register_arm(const std::string& arm_id) {
    JWLOCK_(p_);
    return get_p()->register_arm(arm_id);
  }

  bool delete_arm(const std::string& arm_id) {
    JWLOCK_(p_);
    return get_p()->delete_arm(arm_id);
  }

  std::string select_arm(const std::string& player_id) {
    JWLOCK_(p_);
    return get_p()->select_arm(player_id);
  }

  bool register_reward(const std::string& player_id, const std::string& arm_id,
      double reward) {
    JWLOCK_(p_);
    return get_p()->register_reward(player_id, arm_id, reward);
  }

  std::map<std::string, arm_info> get_arm_info(const std::string& player_id) {
    JRLOCK_(p_);
    return get_p()->get_arm_info(player_id);
  }

  bool reset(const std::string& player_id) {
    JWLOCK_(p_);
    return get_p()->reset(player_id);
  }

  bool clear() {
    JWLOCK_(p_);
    return get_p()->clear();
  }

  std::string get_config() {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  bool save(const std::string& id) {
    JRLOCK_(p_);
    return get_p()->save(id);
  }

  bool load(const std::string& id) {
    JWLOCK_(p_);
    return get_p()->load(id);
  }

  std::map<std::string, std::map<std::string, std::string> > get_status() {
    JRLOCK_(p_);
    return p_->get_status();
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
