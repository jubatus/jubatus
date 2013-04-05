
// This file is auto-generated from nearest_neighbor.idl
// *** DO NOT EDIT ***

#ifndef NEAREST_NEIGHBOR_SERVER_HPP_
#define NEAREST_NEIGHBOR_SERVER_HPP_


#include "nearest_neighbor_types.hpp"
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/bind.h>


namespace jubatus {

namespace server {

template <class Impl>
class nearest_neighbor : public pfi::network::mprpc::rpc_server {
public:
  nearest_neighbor(double timeout_sec): rpc_server(timeout_sec) {

    rpc_server::add<bool(std::string, config_data) >("set_config", pfi::lang::bind(&Impl::set_config, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<config_data(std::string) >("get_config", pfi::lang::bind(&Impl::get_config, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<bool(std::string) >("init_table", pfi::lang::bind(&Impl::init_table, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<bool(std::string) >("clear", pfi::lang::bind(&Impl::clear, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string, datum) >("set_row", pfi::lang::bind(&Impl::set_row, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<neighbor_result(std::string, std::string, uint32_t) >("neighbor_row_from_id", pfi::lang::bind(&Impl::neighbor_row_from_id, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<neighbor_result(std::string, datum, uint32_t) >("neighbor_row_from_data", pfi::lang::bind(&Impl::neighbor_row_from_data, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<neighbor_result(std::string, std::string, int32_t) >("similar_row_from_id", pfi::lang::bind(&Impl::similar_row_from_id, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<neighbor_result(std::string, datum, int32_t) >("similar_row_from_data", pfi::lang::bind(&Impl::similar_row_from_data, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<bool(std::string, std::string) >("save", pfi::lang::bind(&Impl::save, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string) >("load", pfi::lang::bind(&Impl::load, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string > >(std::string) >("get_status", pfi::lang::bind(&Impl::get_status, static_cast<Impl*>(this), pfi::lang::_1));
  }
};

} // namespace server

} // namespace jubatus



#endif // NEAREST_NEIGHBOR_SERVER_HPP_
