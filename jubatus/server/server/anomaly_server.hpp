// This file is auto-generated from anomaly.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_ANOMALY_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_ANOMALY_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/bind.h>

#include "../common/mprpc/rpc_server.hpp"
#include "anomaly_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class anomaly : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit anomaly(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", pfi::lang::bind(
        &Impl::get_config, impl, pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string)>("clear_row",
         pfi::lang::bind(&Impl::clear_row, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::pair<std::string, float>(std::string, datum)>("add",
         pfi::lang::bind(&Impl::add, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<float(std::string, std::string, datum)>("update",
         pfi::lang::bind(&Impl::update, impl, pfi::lang::_1, pfi::lang::_2,
         pfi::lang::_3));
    rpc_server::add<bool(std::string)>("clear", pfi::lang::bind(&Impl::clear,
         impl, pfi::lang::_1));
    rpc_server::add<float(std::string, datum)>("calc_score", pfi::lang::bind(
        &Impl::calc_score, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::vector<std::string>(std::string)>("get_all_rows",
         pfi::lang::bind(&Impl::get_all_rows, impl, pfi::lang::_1));
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

#endif  // JUBATUS_SERVER_SERVER_ANOMALY_SERVER_HPP_
