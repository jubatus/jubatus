// This file is auto-generated from weight.idl(0.9.0-24-gda61383) with jenerator version 0.8.5-6-g5a2c923/develop
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "weight_serv.hpp"

namespace jubatus {
namespace server {

class weight_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit weight_impl(const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<weight_serv>(a, false)) {

    rpc_server::add<std::vector<feature>(std::string,
        jubatus::core::fv_converter::datum)>("update",
        jubatus::util::lang::bind(&weight_impl::update, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::vector<feature>(std::string,
        jubatus::core::fv_converter::datum)>("calc_weight",
        jubatus::util::lang::bind(&weight_impl::calc_weight, this,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(
        &weight_impl::clear, this));

    rpc_server::add<std::string(std::string)>("get_config",
        jubatus::util::lang::bind(&weight_impl::get_config, this));
    rpc_server::add<std::map<std::string, std::string>(std::string,
        std::string)>("save", jubatus::util::lang::bind(&weight_impl::save,
        this, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load",
        jubatus::util::lang::bind(&weight_impl::load, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", jubatus::util::lang::bind(
        &weight_impl::get_status, this));
  }

  std::vector<feature> update(const jubatus::core::fv_converter::datum& d) {
    NOLOCK_(p_);
    return get_p()->update(d);
  }

  std::vector<feature> calc_weight(
      const jubatus::core::fv_converter::datum& d) {
    NOLOCK_(p_);
    return get_p()->calc_weight(d);
  }

  bool clear() {
    NOLOCK_(p_);
    return get_p()->clear();
  }

  std::string get_config() {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  std::map<std::string, std::string> save(const std::string& id) {
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
  jubatus::util::lang::shared_ptr<weight_serv> get_p() { return p_->server(); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<weight_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::weight_impl,
        jubatus::server::weight_serv>
      (argc, argv, "weight");
}
