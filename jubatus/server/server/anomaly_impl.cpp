// This file is auto-generated from anomaly.idl(0.4.5-331-gd7b15b4) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/shared_ptr.h>

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
        pfi::lang::bind(&anomaly_impl::clear_row, this, pfi::lang::_2));
    rpc_server::add<id_with_score(std::string,
        jubatus::core::fv_converter::datum)>("add", pfi::lang::bind(
        &anomaly_impl::add, this, pfi::lang::_2));
    rpc_server::add<float(std::string, std::string,
        jubatus::core::fv_converter::datum)>("update", pfi::lang::bind(
        &anomaly_impl::update, this, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<float(std::string, std::string,
        jubatus::core::fv_converter::datum)>("overwrite", pfi::lang::bind(
        &anomaly_impl::overwrite, this, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<bool(std::string)>("clear", pfi::lang::bind(
        &anomaly_impl::clear, this));
    rpc_server::add<float(std::string, jubatus::core::fv_converter::datum)>(
        "calc_score", pfi::lang::bind(&anomaly_impl::calc_score, this,
        pfi::lang::_2));
    rpc_server::add<std::vector<std::string>(std::string)>("get_all_rows",
        pfi::lang::bind(&anomaly_impl::get_all_rows, this));

    rpc_server::add<std::string(std::string)>("get_config", pfi::lang::bind(
        &anomaly_impl::get_config, this));
    rpc_server::add<bool(std::string, std::string)>("save", pfi::lang::bind(
        &anomaly_impl::save, this, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load", pfi::lang::bind(
        &anomaly_impl::load, this, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", pfi::lang::bind(&anomaly_impl::get_status,
        this));
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
  pfi::lang::shared_ptr<anomaly_serv> get_p() { return p_->server(); }

 private:
  pfi::lang::shared_ptr<jubatus::server::framework::server_helper<anomaly_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::anomaly_impl>
      (argc, argv, "anomaly");
}
