// This file is auto-generated from stat.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_STAT_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_STAT_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/bind.h"

#include "../../server/common/mprpc/rpc_server.hpp"
#include "stat_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class stat : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit stat(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", jubatus::util::lang::bind(
        &Impl::get_config, impl, jubatus::util::lang::_1));
    rpc_server::add<bool(std::string, std::string, double)>("push",
        jubatus::util::lang::bind(&Impl::push, impl, jubatus::util::lang::_1, jubatus::util::lang::_2,
        jubatus::util::lang::_3));
    rpc_server::add<double(std::string, std::string)>("sum", jubatus::util::lang::bind(
        &Impl::sum, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<double(std::string, std::string)>("stddev", jubatus::util::lang::bind(
        &Impl::stddev, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<double(std::string, std::string)>("max", jubatus::util::lang::bind(
        &Impl::max, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<double(std::string, std::string)>("min", jubatus::util::lang::bind(
        &Impl::min, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<double(std::string, std::string)>("entropy",
        jubatus::util::lang::bind(&Impl::entropy, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<double(std::string, std::string, int32_t, double)>("moment",
        jubatus::util::lang::bind(&Impl::moment, impl, jubatus::util::lang::_1, jubatus::util::lang::_2,
        jubatus::util::lang::_3, jubatus::util::lang::_4));
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(&Impl::clear,
        impl, jubatus::util::lang::_1));
    rpc_server::add<bool(std::string, std::string)>("save", jubatus::util::lang::bind(
        &Impl::save, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load", jubatus::util::lang::bind(
        &Impl::load, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", jubatus::util::lang::bind(&Impl::get_status, impl,
        jubatus::util::lang::_1));
  }
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_STAT_SERVER_HPP_
