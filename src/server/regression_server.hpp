
// This file is auto-generated from regression.idl
// *** DO NOT EDIT ***

#ifndef REGRESSION_SERVER_HPP_
#define REGRESSION_SERVER_HPP_


#include "regression_types.hpp"
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/bind.h>
#include "../common/mprpc/rpc_server.hpp"


namespace jubatus {

namespace server {

template <class Impl>
class regression : public jubatus::common::mprpc::rpc_server {
public:
  regression(double timeout_sec): rpc_server(timeout_sec) {

    rpc_server::add<bool(std::string, config_data) >("set_config", pfi::lang::bind(&Impl::set_config, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<config_data(std::string) >("get_config", pfi::lang::bind(&Impl::get_config, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<int32_t(std::string, std::vector<std::pair<float, datum > >) >("train", pfi::lang::bind(&Impl::train, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::vector<float >(std::string, std::vector<datum >) >("estimate", pfi::lang::bind(&Impl::estimate, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string) >("save", pfi::lang::bind(&Impl::save, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string) >("load", pfi::lang::bind(&Impl::load, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string > >(std::string) >("get_status", pfi::lang::bind(&Impl::get_status, static_cast<Impl*>(this), pfi::lang::_1));
  }
};

} // namespace server

} // namespace jubatus



#endif // REGRESSION_SERVER_HPP_
