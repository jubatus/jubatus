// This file is auto-generated from nearest_neighbor.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_NEAREST_NEIGHBOR_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_NEAREST_NEIGHBOR_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/bind.h>

#include "../../server/common/mprpc/rpc_server.hpp"
#include "nearest_neighbor_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class nearest_neighbor : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit nearest_neighbor(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<bool(std::string)>("init_table", pfi::lang::bind(
        &Impl::init_table, impl, pfi::lang::_1));
    rpc_server::add<bool(std::string)>("clear", pfi::lang::bind(&Impl::clear,
         impl, pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string,
         jubatus::core::fv_converter::datum)>("set_row", pfi::lang::bind(
        &Impl::set_row, impl, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<neighbor_result(std::string, std::string, uint32_t)>(
        "neighbor_row_from_id", pfi::lang::bind(&Impl::neighbor_row_from_id,
         impl, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<neighbor_result(std::string,
         jubatus::core::fv_converter::datum, uint32_t)>(
        "neighbor_row_from_data", pfi::lang::bind(&Impl::neighbor_row_from_data,
         impl, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<neighbor_result(std::string, std::string, int32_t)>(
        "similar_row_from_id", pfi::lang::bind(&Impl::similar_row_from_id, impl,
         pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<neighbor_result(std::string,
         jubatus::core::fv_converter::datum, int32_t)>("similar_row_from_data",
         pfi::lang::bind(&Impl::similar_row_from_data, impl, pfi::lang::_1,
         pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<bool(std::string, std::string)>("save", pfi::lang::bind(
        &Impl::save, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load", pfi::lang::bind(
        &Impl::load, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", pfi::lang::bind(&Impl::get_status, impl,
         pfi::lang::_1));
    rpc_server::add<std::string(std::string)>("get_config", pfi::lang::bind(
        &Impl::get_config, impl, pfi::lang::_1));
  }
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_NEAREST_NEIGHBOR_SERVER_HPP_
