
// This file is auto-generated from recommender.idl
// *** DO NOT EDIT ***

#ifndef RECOMMENDER_SERVER_HPP_
#define RECOMMENDER_SERVER_HPP_


#include "recommender_types.hpp"
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/bind.h>


namespace jubatus {

namespace server {

template <class Impl>
class recommender : public pfi::network::mprpc::rpc_server {
public:
  recommender(double timeout_sec): rpc_server(timeout_sec) {

    rpc_server::add<int32_t(std::string, config_data) >("set_config", pfi::lang::bind(&Impl::set_config, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<config_data(std::string, int32_t) >("get_config", pfi::lang::bind(&Impl::get_config, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<int32_t(std::string, std::string, int32_t) >("clear_row", pfi::lang::bind(&Impl::clear_row, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<int32_t(std::string, std::string, datum) >("update_row", pfi::lang::bind(&Impl::update_row, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<int32_t(std::string, int32_t) >("clear", pfi::lang::bind(&Impl::clear, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<datum(std::string, std::string, int32_t) >("complete_row_from_id", pfi::lang::bind(&Impl::complete_row_from_id, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<datum(std::string, datum) >("complete_row_from_data", pfi::lang::bind(&Impl::complete_row_from_data, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<similar_result(std::string, std::string, size_t) >("similar_row_from_id", pfi::lang::bind(&Impl::similar_row_from_id, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<similar_result(std::string, std::pair<datum, size_t >) >("similar_row_from_data", pfi::lang::bind(&Impl::similar_row_from_data, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<datum(std::string, std::string, int32_t) >("decode_row", pfi::lang::bind(&Impl::decode_row, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<std::vector<std::string >(std::string, int32_t) >("get_all_rows", pfi::lang::bind(&Impl::get_all_rows, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<float(std::string, std::pair<datum, datum >) >("similarity", pfi::lang::bind(&Impl::similarity, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<float(std::string, datum) >("l2norm", pfi::lang::bind(&Impl::l2norm, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<int32_t(std::string, std::string) >("save", pfi::lang::bind(&Impl::save, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<int32_t(std::string, std::string) >("load", pfi::lang::bind(&Impl::load, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string > >(std::string, int32_t) >("get_status", pfi::lang::bind(&Impl::get_status, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
  }
};

} // namespace server

} // namespace jubatus



#endif // RECOMMENDER_SERVER_HPP_
