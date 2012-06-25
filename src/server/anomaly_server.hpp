
// This file is auto-generated from anomaly.idl
// *** DO NOT EDIT ***

#ifndef ANOMALY_SERVER_HPP_
#define ANOMALY_SERVER_HPP_


#include "anomaly_types.hpp"
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/bind.h>


namespace jubatus {

namespace server {

template <class Impl>
class anomaly : public pfi::network::mprpc::rpc_server {
public:
  anomaly(double timeout_sec): rpc_server(timeout_sec) {

    rpc_server::add<bool(std::string, config_data) >("set_config", pfi::lang::bind(&Impl::set_config, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<config_data(std::string) >("get_config", pfi::lang::bind(&Impl::get_config, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string) >("clear_row", pfi::lang::bind(&Impl::clear_row, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::pair<std::string, float >(std::string, datum) >("add", pfi::lang::bind(&Impl::add, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<float(std::string, std::string, datum) >("update", pfi::lang::bind(&Impl::update, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<bool(std::string) >("clear", pfi::lang::bind(&Impl::clear, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<float(std::string, datum) >("calc_score", pfi::lang::bind(&Impl::calc_score, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::vector<std::string >(std::string) >("get_all_rows", pfi::lang::bind(&Impl::get_all_rows, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string) >("save", pfi::lang::bind(&Impl::save, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string) >("load", pfi::lang::bind(&Impl::load, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string > >(std::string) >("get_status", pfi::lang::bind(&Impl::get_status, static_cast<Impl*>(this), pfi::lang::_1));
  }
};

} // namespace server

} // namespace jubatus



#endif // ANOMALY_SERVER_HPP_
