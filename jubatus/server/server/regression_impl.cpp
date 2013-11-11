// This file is auto-generated from regression.idl(0.4.3-191-gd1afc2f) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/shared_ptr.h>

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
        pfi::lang::bind(&regression_impl::train, this, pfi::lang::_2));
    rpc_server::add<std::vector<float>(std::string,
        std::vector<jubatus::core::fv_converter::datum>)>("estimate",
        pfi::lang::bind(&regression_impl::estimate, this, pfi::lang::_2));
    rpc_server::add<bool(std::string)>("clear", pfi::lang::bind(
        &regression_impl::clear, this));

    rpc_server::add<std::string(std::string)>("get_config", pfi::lang::bind(
        &regression_impl::get_config, this));
    rpc_server::add<bool(std::string, std::string)>("save", pfi::lang::bind(
        &regression_impl::save, this, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load", pfi::lang::bind(
        &regression_impl::load, this, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", pfi::lang::bind(
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
    JWLOCK_(p_);
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
  pfi::lang::shared_ptr<regression_serv> get_p() { return p_->server(); }

 private:
  pfi::lang::shared_ptr<jubatus::server::framework::server_helper<regression_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::regression_impl>
      (argc, argv, "regression");
}
