// This file is auto-generated from nearest_neighbor.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_NEAREST_NEIGHBOR_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_NEAREST_NEIGHBOR_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/bind.h"

#include "../../server/common/mprpc/rpc_server.hpp"
#include "nearest_neighbor_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class nearest_neighbor : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit nearest_neighbor(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(&Impl::clear,
        impl, jubatus::util::lang::_1));
    rpc_server::add<bool(std::string, std::string,
        jubatus::core::fv_converter::datum)>("set_row", jubatus::util::lang::bind(
        &Impl::set_row, impl, jubatus::util::lang::_1, jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<neighbor_result(std::string, std::string, uint32_t)>(
        "neighbor_row_from_id", jubatus::util::lang::bind(&Impl::neighbor_row_from_id,
        impl, jubatus::util::lang::_1, jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<neighbor_result(std::string,
        jubatus::core::fv_converter::datum, uint32_t)>("neighbor_row_from_data",
        jubatus::util::lang::bind(&Impl::neighbor_row_from_data, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<neighbor_result(std::string, std::string, int32_t)>(
        "similar_row_from_id", jubatus::util::lang::bind(&Impl::similar_row_from_id, impl,
        jubatus::util::lang::_1, jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<neighbor_result(std::string,
        jubatus::core::fv_converter::datum, int32_t)>("similar_row_from_data",
        jubatus::util::lang::bind(&Impl::similar_row_from_data, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<bool(std::string, std::string)>("save", jubatus::util::lang::bind(
        &Impl::save, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load", jubatus::util::lang::bind(
        &Impl::load, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", jubatus::util::lang::bind(&Impl::get_status, impl,
        jubatus::util::lang::_1));
    rpc_server::add<std::string(std::string)>("get_config", jubatus::util::lang::bind(
        &Impl::get_config, impl, jubatus::util::lang::_1));
  }
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_NEAREST_NEIGHBOR_SERVER_HPP_
