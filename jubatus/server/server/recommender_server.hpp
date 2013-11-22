// This file is auto-generated from recommender.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_RECOMMENDER_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_RECOMMENDER_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/bind.h"

#include "../../server/common/mprpc/rpc_server.hpp"
#include "recommender_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class recommender : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit recommender(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", jubatus::util::lang::bind(
        &Impl::get_config, impl, jubatus::util::lang::_1));
    rpc_server::add<bool(std::string, std::string)>("clear_row",
        jubatus::util::lang::bind(&Impl::clear_row, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string,
        jubatus::core::fv_converter::datum)>("update_row", jubatus::util::lang::bind(
        &Impl::update_row, impl, jubatus::util::lang::_1, jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(&Impl::clear,
        impl, jubatus::util::lang::_1));
    rpc_server::add<jubatus::core::fv_converter::datum(std::string,
        std::string)>("complete_row_from_id", jubatus::util::lang::bind(
        &Impl::complete_row_from_id, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<jubatus::core::fv_converter::datum(std::string,
        jubatus::core::fv_converter::datum)>("complete_row_from_datum",
        jubatus::util::lang::bind(&Impl::complete_row_from_datum, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2));
    rpc_server::add<similar_result(std::string, std::string, uint32_t)>(
        "similar_row_from_id", jubatus::util::lang::bind(&Impl::similar_row_from_id, impl,
        jubatus::util::lang::_1, jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<similar_result(std::string,
        jubatus::core::fv_converter::datum, uint32_t)>("similar_row_from_datum",
        jubatus::util::lang::bind(&Impl::similar_row_from_datum, impl, jubatus::util::lang::_1,
        jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<jubatus::core::fv_converter::datum(std::string,
        std::string)>("decode_row", jubatus::util::lang::bind(&Impl::decode_row, impl,
        jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<std::vector<std::string>(std::string)>("get_all_rows",
        jubatus::util::lang::bind(&Impl::get_all_rows, impl, jubatus::util::lang::_1));
    rpc_server::add<float(std::string, jubatus::core::fv_converter::datum,
        jubatus::core::fv_converter::datum)>("calc_similarity", jubatus::util::lang::bind(
        &Impl::calc_similarity, impl, jubatus::util::lang::_1, jubatus::util::lang::_2,
        jubatus::util::lang::_3));
    rpc_server::add<float(std::string, jubatus::core::fv_converter::datum)>(
        "calc_l2norm", jubatus::util::lang::bind(&Impl::calc_l2norm, impl, jubatus::util::lang::_1,
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

#endif  // JUBATUS_SERVER_SERVER_RECOMMENDER_SERVER_HPP_
