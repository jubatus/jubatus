// This file is auto-generated from stat.idl(0.6.4-33-gcc8d7ca) with jenerator version 0.6.4-107-ge23137f/undefined
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "stat_serv.hpp"

namespace jubatus {
namespace server {

class stat_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit stat_impl(const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<stat_serv>(a, true)) {

    rpc_server::add("push", jubatus::util::lang::bind(&stat_impl::push, this,
        jubatus::util::lang::_1));
    rpc_server::add("sum", jubatus::util::lang::bind(&stat_impl::sum, this,
        jubatus::util::lang::_1));
    rpc_server::add("stddev", jubatus::util::lang::bind(&stat_impl::stddev,
        this, jubatus::util::lang::_1));
    rpc_server::add("max", jubatus::util::lang::bind(&stat_impl::max, this,
        jubatus::util::lang::_1));
    rpc_server::add("min", jubatus::util::lang::bind(&stat_impl::min, this,
        jubatus::util::lang::_1));
    rpc_server::add("entropy", jubatus::util::lang::bind(&stat_impl::entropy,
        this, jubatus::util::lang::_1));
    rpc_server::add("moment", jubatus::util::lang::bind(&stat_impl::moment,
        this, jubatus::util::lang::_1));
    rpc_server::add("clear", jubatus::util::lang::bind(&stat_impl::clear, this,
        jubatus::util::lang::_1));

    rpc_server::add("get_config", jubatus::util::lang::bind(
        &stat_impl::get_config, this, jubatus::util::lang::_1));
    rpc_server::add("save", jubatus::util::lang::bind(&stat_impl::save, this,
        jubatus::util::lang::_1));
    rpc_server::add("load", jubatus::util::lang::bind(&stat_impl::load, this,
        jubatus::util::lang::_1));
    rpc_server::add("get_status", jubatus::util::lang::bind(
        &stat_impl::get_status, this, jubatus::util::lang::_1));
  }

  void push(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string, double> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->push(params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void sum(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    double retval = get_p()->sum(params.get<1>());
    req.result(retval);
  }

  void stddev(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    double retval = get_p()->stddev(params.get<1>());
    req.result(retval);
  }

  void max(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    double retval = get_p()->max(params.get<1>());
    req.result(retval);
  }

  void min(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    double retval = get_p()->min(params.get<1>());
    req.result(retval);
  }

  void entropy(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    double retval = get_p()->entropy(params.get<1>());
    req.result(retval);
  }

  void moment(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string, int32_t, double> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    double retval = get_p()->moment(params.get<1>(), params.get<2>(),
        params.get<3>());
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
  jubatus::util::lang::shared_ptr<stat_serv> get_p() { return p_->server(); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<stat_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::stat_impl>
      (argc, argv, "stat");
}
