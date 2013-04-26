// This file is auto-generated from classifier.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_CLASSIFIER_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_CLASSIFIER_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/bind.h>

#include "../common/mprpc/rpc_server.hpp"
#include "classifier_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class classifier : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit classifier(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", pfi::lang::bind(
        &Impl::get_config, impl, pfi::lang::_1));
    rpc_server::add<int32_t(std::string, std::vector<std::pair<std::string,
         datum> >)>("train", pfi::lang::bind(&Impl::train, impl, pfi::lang::_1,
         pfi::lang::_2));
    rpc_server::add<std::vector<std::vector<estimate_result> >(std::string,
         std::vector<datum>)>("classify", pfi::lang::bind(&Impl::classify, impl,
         pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string)>("clear", pfi::lang::bind(&Impl::clear,
         impl, pfi::lang::_1));
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

#endif  // JUBATUS_SERVER_SERVER_CLASSIFIER_SERVER_HPP_
