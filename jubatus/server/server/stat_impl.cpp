// This file is auto-generated from stat.idl(0.5.2-68-g68e898d) with jenerator version 0.5.4-224-g49229fa/develop
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

    rpc_server::add<bool(std::string, std::string, double)>("push",
        jubatus::util::lang::bind(&stat_impl::push, this,
        jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<double(std::string, std::string)>("sum",
        jubatus::util::lang::bind(&stat_impl::sum, this,
        jubatus::util::lang::_2));
    rpc_server::add<double(std::string, std::string)>("stddev",
        jubatus::util::lang::bind(&stat_impl::stddev, this,
        jubatus::util::lang::_2));
    rpc_server::add<double(std::string, std::string)>("max",
        jubatus::util::lang::bind(&stat_impl::max, this,
        jubatus::util::lang::_2));
    rpc_server::add<double(std::string, std::string)>("min",
        jubatus::util::lang::bind(&stat_impl::min, this,
        jubatus::util::lang::_2));
    rpc_server::add<double(std::string, std::string)>("entropy",
        jubatus::util::lang::bind(&stat_impl::entropy, this,
        jubatus::util::lang::_2));
    rpc_server::add<double(std::string, std::string, int32_t, double)>("moment",
        jubatus::util::lang::bind(&stat_impl::moment, this,
        jubatus::util::lang::_2, jubatus::util::lang::_3,
        jubatus::util::lang::_4));
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(
        &stat_impl::clear, this));

    rpc_server::add<std::string(std::string)>("get_config",
        jubatus::util::lang::bind(&stat_impl::get_config, this));
    rpc_server::add<bool(std::string, std::string)>("save",
        jubatus::util::lang::bind(&stat_impl::save, this,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load",
        jubatus::util::lang::bind(&stat_impl::load, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", jubatus::util::lang::bind(
        &stat_impl::get_status, this));
  }

  bool push(const std::string& key, double value) {
    JWLOCK_(p_);
    return get_p()->push(key, value);
  }

  double sum(const std::string& key) {
    JRLOCK_(p_);
    return get_p()->sum(key);
  }

  double stddev(const std::string& key) {
    JRLOCK_(p_);
    return get_p()->stddev(key);
  }

  double max(const std::string& key) {
    JRLOCK_(p_);
    return get_p()->max(key);
  }

  double min(const std::string& key) {
    JRLOCK_(p_);
    return get_p()->min(key);
  }

  double entropy(const std::string& key) {
    JRLOCK_(p_);
    return get_p()->entropy(key);
  }

  double moment(const std::string& key, int32_t degree, double center) {
    JRLOCK_(p_);
    return get_p()->moment(key, degree, center);
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
