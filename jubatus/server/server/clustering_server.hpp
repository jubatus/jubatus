// This file is auto-generated from clustering.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_CLUSTERING_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_CLUSTERING_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/bind.h"

#include "../../server/common/mprpc/rpc_server.hpp"
#include "clustering_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class clustering : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit clustering(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", jubatus::util::lang::bind(
        &Impl::get_config, impl, jubatus::util::lang::_1));
    rpc_server::add<bool(std::string,
        std::vector<jubatus::core::fv_converter::datum>)>("push",
        jubatus::util::lang::bind(&Impl::push, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<uint32_t(std::string)>("get_revision", jubatus::util::lang::bind(
        &Impl::get_revision, impl, jubatus::util::lang::_1));
    rpc_server::add<std::vector<std::vector<std::pair<double,
        jubatus::core::fv_converter::datum> > >(std::string)>(
        "get_core_members", jubatus::util::lang::bind(&Impl::get_core_members, impl,
        jubatus::util::lang::_1));
    rpc_server::add<std::vector<jubatus::core::fv_converter::datum>(
        std::string)>("get_k_center", jubatus::util::lang::bind(&Impl::get_k_center, impl,
        jubatus::util::lang::_1));
    rpc_server::add<jubatus::core::fv_converter::datum(std::string,
        jubatus::core::fv_converter::datum)>("get_nearest_center",
        jubatus::util::lang::bind(&Impl::get_nearest_center, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2));
    rpc_server::add<std::vector<std::pair<double,
        jubatus::core::fv_converter::datum> >(std::string,
        jubatus::core::fv_converter::datum)>("get_nearest_members",
        jubatus::util::lang::bind(&Impl::get_nearest_members, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2));
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

#endif  // JUBATUS_SERVER_SERVER_CLUSTERING_SERVER_HPP_
