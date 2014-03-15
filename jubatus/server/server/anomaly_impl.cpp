// This file is auto-generated from anomaly.idl(0.6.4-60-gdff9eb0) with jenerator version 0.6.4-107-ge23137f/undefined
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

    rpc_server::add("clear_row", jubatus::util::lang::bind(
        &anomaly_impl::clear_row, this, jubatus::util::lang::_1));
    rpc_server::add("add", jubatus::util::lang::bind(&anomaly_impl::add, this,
        jubatus::util::lang::_1));
    rpc_server::add("update", jubatus::util::lang::bind(&anomaly_impl::update,
        this, jubatus::util::lang::_1));
    rpc_server::add("overwrite", jubatus::util::lang::bind(
        &anomaly_impl::overwrite, this, jubatus::util::lang::_1));
    rpc_server::add("clear", jubatus::util::lang::bind(&anomaly_impl::clear,
        this, jubatus::util::lang::_1));
    rpc_server::add("calc_score", jubatus::util::lang::bind(
        &anomaly_impl::calc_score, this, jubatus::util::lang::_1));
    rpc_server::add("get_all_rows", jubatus::util::lang::bind(
        &anomaly_impl::get_all_rows, this, jubatus::util::lang::_1));

    rpc_server::add("get_config", jubatus::util::lang::bind(
        &anomaly_impl::get_config, this, jubatus::util::lang::_1));
    rpc_server::add("save", jubatus::util::lang::bind(&anomaly_impl::save, this,
        jubatus::util::lang::_1));
    rpc_server::add("load", jubatus::util::lang::bind(&anomaly_impl::load, this,
        jubatus::util::lang::_1));
    rpc_server::add("get_status", jubatus::util::lang::bind(
        &anomaly_impl::get_status, this, jubatus::util::lang::_1));
  }

  void clear_row(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->clear_row(params.get<1>());
    req.result(retval);
  }

  void add(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        jubatus::core::fv_converter::datum> params;
    req.params().convert(&params);
    NOLOCK_(p_);
    id_with_score retval = get_p()->add(params.get<1>());
    req.result(retval);
  }

  void update(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string,
        jubatus::core::fv_converter::datum> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    float retval = get_p()->update(params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void overwrite(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string,
        jubatus::core::fv_converter::datum> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    float retval = get_p()->overwrite(params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void clear(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->clear();
    req.result(retval);
  }

  void calc_score(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        jubatus::core::fv_converter::datum> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    float retval = get_p()->calc_score(params.get<1>());
    req.result(retval);
  }

  void get_all_rows(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<std::string> retval = get_p()->get_all_rows();
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
