// This file is auto-generated from anomaly.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_ANOMALY_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_ANOMALY_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/bind.h"

#include "../../server/common/mprpc/rpc_server.hpp"
#include "anomaly_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class anomaly : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit anomaly(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", jubatus::util::lang::bind(
        &Impl::get_config, impl, jubatus::util::lang::_1));
    rpc_server::add<bool(std::string, std::string)>("clear_row",
        jubatus::util::lang::bind(&Impl::clear_row, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<std::pair<std::string, float>(std::string,
        jubatus::core::fv_converter::datum)>("add", jubatus::util::lang::bind(&Impl::add,
        impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<float(std::string, std::string,
        jubatus::core::fv_converter::datum)>("update", jubatus::util::lang::bind(
        &Impl::update, impl, jubatus::util::lang::_1, jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<float(std::string, std::string,
        jubatus::core::fv_converter::datum)>("overwrite", jubatus::util::lang::bind(
        &Impl::overwrite, impl, jubatus::util::lang::_1, jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(&Impl::clear,
        impl, jubatus::util::lang::_1));
    rpc_server::add<float(std::string, jubatus::core::fv_converter::datum)>(
        "calc_score", jubatus::util::lang::bind(&Impl::calc_score, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2));
    rpc_server::add<std::vector<std::string>(std::string)>("get_all_rows",
        jubatus::util::lang::bind(&Impl::get_all_rows, impl, jubatus::util::lang::_1));
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

#endif  // JUBATUS_SERVER_SERVER_ANOMALY_SERVER_HPP_
