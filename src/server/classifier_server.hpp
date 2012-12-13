
// This file is auto-generated from classifier.idl
// *** DO NOT EDIT ***

#ifndef CLASSIFIER_SERVER_HPP_
#define CLASSIFIER_SERVER_HPP_


#include "classifier_types.hpp"
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/bind.h>


namespace jubatus {

namespace server {

template <class Impl>
class classifier : public pfi::network::mprpc::rpc_server {
public:
  classifier(double timeout_sec): rpc_server(timeout_sec) {

    rpc_server::add<bool(std::string, std::string) >("set_config", pfi::lang::bind(&Impl::set_config, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::string(std::string) >("get_config", pfi::lang::bind(&Impl::get_config, static_cast<Impl*>(this), pfi::lang::_1));
    rpc_server::add<int32_t(std::string, std::vector<std::pair<std::string, datum > >) >("train", pfi::lang::bind(&Impl::train, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::vector<std::vector<estimate_result > >(std::string, std::vector<datum >) >("classify", pfi::lang::bind(&Impl::classify, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string) >("save", pfi::lang::bind(&Impl::save, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string) >("load", pfi::lang::bind(&Impl::load, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string > >(std::string) >("get_status", pfi::lang::bind(&Impl::get_status, static_cast<Impl*>(this), pfi::lang::_1));
  }
};

} // namespace server

} // namespace jubatus



#endif // CLASSIFIER_SERVER_HPP_
