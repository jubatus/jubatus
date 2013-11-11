// This file is auto-generated from clustering.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_CLUSTERING_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_CLUSTERING_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/bind.h>

#include "../../server/common/mprpc/rpc_server.hpp"
#include "clustering_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class clustering : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit clustering(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", pfi::lang::bind(
        &Impl::get_config, impl, pfi::lang::_1));
    rpc_server::add<bool(std::string,
        std::vector<jubatus::core::fv_converter::datum>)>("push",
        pfi::lang::bind(&Impl::push, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<uint32_t(std::string)>("get_revision", pfi::lang::bind(
        &Impl::get_revision, impl, pfi::lang::_1));
    rpc_server::add<std::vector<std::vector<std::pair<double,
        jubatus::core::fv_converter::datum> > >(std::string)>(
        "get_core_members", pfi::lang::bind(&Impl::get_core_members, impl,
        pfi::lang::_1));
    rpc_server::add<std::vector<jubatus::core::fv_converter::datum>(
        std::string)>("get_k_center", pfi::lang::bind(&Impl::get_k_center, impl,
        pfi::lang::_1));
    rpc_server::add<jubatus::core::fv_converter::datum(std::string,
        jubatus::core::fv_converter::datum)>("get_nearest_center",
        pfi::lang::bind(&Impl::get_nearest_center, impl, pfi::lang::_1,
        pfi::lang::_2));
    rpc_server::add<std::vector<std::pair<double,
        jubatus::core::fv_converter::datum> >(std::string,
        jubatus::core::fv_converter::datum)>("get_nearest_members",
        pfi::lang::bind(&Impl::get_nearest_members, impl, pfi::lang::_1,
        pfi::lang::_2));
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

#endif  // JUBATUS_SERVER_SERVER_CLUSTERING_SERVER_HPP_
