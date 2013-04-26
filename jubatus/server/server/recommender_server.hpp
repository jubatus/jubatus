// This file is auto-generated from recommender.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_RECOMMENDER_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_RECOMMENDER_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/bind.h>

#include "../common/mprpc/rpc_server.hpp"
#include "recommender_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class recommender : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit recommender(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", pfi::lang::bind(
        &Impl::get_config, impl, pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string)>("clear_row",
         pfi::lang::bind(&Impl::clear_row, impl, pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string, datum)>("update_row",
         pfi::lang::bind(&Impl::update_row, impl, pfi::lang::_1, pfi::lang::_2,
         pfi::lang::_3));
    rpc_server::add<bool(std::string)>("clear", pfi::lang::bind(&Impl::clear,
         impl, pfi::lang::_1));
    rpc_server::add<datum(std::string, std::string)>("complete_row_from_id",
         pfi::lang::bind(&Impl::complete_row_from_id, impl, pfi::lang::_1,
         pfi::lang::_2));
    rpc_server::add<datum(std::string, datum)>("complete_row_from_datum",
         pfi::lang::bind(&Impl::complete_row_from_datum, impl, pfi::lang::_1,
         pfi::lang::_2));
    rpc_server::add<similar_result(std::string, std::string, uint32_t)>(
        "similar_row_from_id", pfi::lang::bind(&Impl::similar_row_from_id, impl,
         pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<similar_result(std::string, datum, uint32_t)>(
        "similar_row_from_datum", pfi::lang::bind(&Impl::similar_row_from_datum,
         impl, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<datum(std::string, std::string)>("decode_row",
         pfi::lang::bind(&Impl::decode_row, impl, pfi::lang::_1,
         pfi::lang::_2));
    rpc_server::add<std::vector<std::string>(std::string)>("get_all_rows",
         pfi::lang::bind(&Impl::get_all_rows, impl, pfi::lang::_1));
    rpc_server::add<float(std::string, datum, datum)>("calc_similarity",
         pfi::lang::bind(&Impl::calc_similarity, impl, pfi::lang::_1,
         pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<float(std::string, datum)>("calc_l2norm", pfi::lang::bind(
        &Impl::calc_l2norm, impl, pfi::lang::_1, pfi::lang::_2));
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

#endif  // JUBATUS_SERVER_SERVER_RECOMMENDER_SERVER_HPP_
