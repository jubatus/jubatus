// This file is auto-generated from anomaly.idl(0.5.2-68-g68e898d) with jenerator version 0.5.4-224-g49229fa/develop
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "anomaly_serv.hpp"

namespace jubatus {
namespace server {

class anomaly_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit anomaly_impl(const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<anomaly_serv>(a, true)) {

    rpc_server::add<bool(std::string, std::string)>("clear_row",
        jubatus::util::lang::bind(&anomaly_impl::clear_row, this,
        jubatus::util::lang::_2));
    rpc_server::add<id_with_score(std::string,
        jubatus::core::fv_converter::datum)>("add", jubatus::util::lang::bind(
        &anomaly_impl::add, this, jubatus::util::lang::_2));
    rpc_server::add<float(std::string, std::string,
        jubatus::core::fv_converter::datum)>("update",
        jubatus::util::lang::bind(&anomaly_impl::update, this,
        jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<float(std::string, std::string,
        jubatus::core::fv_converter::datum)>("overwrite",
        jubatus::util::lang::bind(&anomaly_impl::overwrite, this,
        jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(
        &anomaly_impl::clear, this));
    rpc_server::add<float(std::string, jubatus::core::fv_converter::datum)>(
        "calc_score", jubatus::util::lang::bind(&anomaly_impl::calc_score, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::vector<std::string>(std::string)>("get_all_rows",
        jubatus::util::lang::bind(&anomaly_impl::get_all_rows, this));

    rpc_server::add<std::string(std::string)>("get_config",
        jubatus::util::lang::bind(&anomaly_impl::get_config, this));
    rpc_server::add<bool(std::string, std::string)>("save",
        jubatus::util::lang::bind(&anomaly_impl::save, this,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load",
        jubatus::util::lang::bind(&anomaly_impl::load, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", jubatus::util::lang::bind(
        &anomaly_impl::get_status, this));
  }

  bool clear_row(const std::string& id) {
    JWLOCK_(p_);
    return get_p()->clear_row(id);
  }

  id_with_score add(const jubatus::core::fv_converter::datum& row) {
    NOLOCK_(p_);
    return get_p()->add(row);
  }

  float update(const std::string& id,
      const jubatus::core::fv_converter::datum& row) {
    JWLOCK_(p_);
    return get_p()->update(id, row);
  }

  float overwrite(const std::string& id,
      const jubatus::core::fv_converter::datum& row) {
    JWLOCK_(p_);
    return get_p()->overwrite(id, row);
  }

  bool clear() {
    JWLOCK_(p_);
    return get_p()->clear();
  }

  float calc_score(const jubatus::core::fv_converter::datum& row) {
    JRLOCK_(p_);
    return get_p()->calc_score(row);
  }

  std::vector<std::string> get_all_rows() {
    JRLOCK_(p_);
    return get_p()->get_all_rows();
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
  jubatus::util::lang::shared_ptr<anomaly_serv> get_p() { return p_->server(); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<anomaly_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::anomaly_impl>
      (argc, argv, "anomaly");
}
