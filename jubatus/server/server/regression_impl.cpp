// This file is auto-generated from regression.idl(0.6.4-33-gcc8d7ca) with jenerator version 0.6.4-107-ge23137f/undefined
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

    rpc_server::add("train", jubatus::util::lang::bind(&regression_impl::train,
        this, jubatus::util::lang::_1));
    rpc_server::add("estimate", jubatus::util::lang::bind(
        &regression_impl::estimate, this, jubatus::util::lang::_1));
    rpc_server::add("clear", jubatus::util::lang::bind(&regression_impl::clear,
        this, jubatus::util::lang::_1));

    rpc_server::add("get_config", jubatus::util::lang::bind(
        &regression_impl::get_config, this, jubatus::util::lang::_1));
    rpc_server::add("save", jubatus::util::lang::bind(&regression_impl::save,
        this, jubatus::util::lang::_1));
    rpc_server::add("load", jubatus::util::lang::bind(&regression_impl::load,
        this, jubatus::util::lang::_1));
    rpc_server::add("get_status", jubatus::util::lang::bind(
        &regression_impl::get_status, this, jubatus::util::lang::_1));
  }

  void train(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::vector<scored_datum> > params;
    req.params().convert(&params);
    JWLOCK_(p_);
    int32_t retval = get_p()->train(params.get<1>());
    req.result(retval);
  }

  void estimate(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        std::vector<jubatus::core::fv_converter::datum> > params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<float> retval = get_p()->estimate(params.get<1>());
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
