// This file is auto-generated from stat.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_STAT_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_STAT_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/bind.h>

#include "../common/mprpc/rpc_server.hpp"
#include "stat_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class stat : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit stat(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", pfi::lang::bind(
        &Impl::get_config, impl, pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string, double)>("push",
         pfi::lang::bind(&Impl::push, impl, pfi::lang::_1, pfi::lang::_2,
         pfi::lang::_3));
    rpc_server::add<double(std::string, std::string)>("sum", pfi::lang::bind(
        &Impl::sum, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<double(std::string, std::string)>("stddev", pfi::lang::bind(
        &Impl::stddev, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<double(std::string, std::string)>("max", pfi::lang::bind(
        &Impl::max, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<double(std::string, std::string)>("min", pfi::lang::bind(
        &Impl::min, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<double(std::string, std::string)>("entropy",
         pfi::lang::bind(&Impl::entropy, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<double(std::string, std::string, int32_t, double)>("moment",
         pfi::lang::bind(&Impl::moment, impl, pfi::lang::_1, pfi::lang::_2,
         pfi::lang::_3, pfi::lang::_4));
    rpc_server::add<bool(std::string)>("clear", pfi::lang::bind(&Impl::clear,
         impl, pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string)>("save", pfi::lang::bind(
        &Impl::save, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load", pfi::lang::bind(
        &Impl::load, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", pfi::lang::bind(&Impl::get_status, impl,
         pfi::lang::_1));
  }
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_STAT_SERVER_HPP_
