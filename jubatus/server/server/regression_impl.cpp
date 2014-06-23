// This file is auto-generated from regression.idl(0.5.2-68-g68e898d) with jenerator version 0.5.4-224-g49229fa/develop
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "regression_serv.hpp"

namespace jubatus {
namespace server {

class regression_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit regression_impl(const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<regression_serv>(a,
        false)) {

    rpc_server::add<int32_t(std::string, std::vector<scored_datum>)>("train",
        jubatus::util::lang::bind(&regression_impl::train, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::vector<float>(std::string,
        std::vector<jubatus::core::fv_converter::datum>)>("estimate",
        jubatus::util::lang::bind(&regression_impl::estimate, this,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(
        &regression_impl::clear, this));

    rpc_server::add<std::string(std::string)>("get_config",
        jubatus::util::lang::bind(&regression_impl::get_config, this));
    rpc_server::add<bool(std::string, std::string)>("save",
        jubatus::util::lang::bind(&regression_impl::save, this,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load",
        jubatus::util::lang::bind(&regression_impl::load, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", jubatus::util::lang::bind(
        &regression_impl::get_status, this));
  }

  int32_t train(const std::vector<scored_datum>& train_data) {
    JWLOCK_(p_);
    return get_p()->train(train_data);
  }

  std::vector<float> estimate(
      const std::vector<jubatus::core::fv_converter::datum>& estimate_data) {
    JRLOCK_(p_);
    return get_p()->estimate(estimate_data);
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
  jubatus::util::lang::shared_ptr<regression_serv> get_p() { return p_->server(
      ); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<regression_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::regression_impl>
      (argc, argv, "regression");
}
