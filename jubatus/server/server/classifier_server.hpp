// This file is auto-generated from classifier.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_CLASSIFIER_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_CLASSIFIER_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/bind.h"

#include "../../server/common/mprpc/rpc_server.hpp"
#include "classifier_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class classifier : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit classifier(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", jubatus::util::lang::bind(
        &Impl::get_config, impl, jubatus::util::lang::_1));
    rpc_server::add<int32_t(std::string, std::vector<std::pair<std::string,
        jubatus::core::fv_converter::datum> >)>("train", jubatus::util::lang::bind(
        &Impl::train, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<std::vector<std::vector<estimate_result> >(std::string,
        std::vector<jubatus::core::fv_converter::datum>)>("classify",
        jubatus::util::lang::bind(&Impl::classify, impl, jubatus::util::lang::_1, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(&Impl::clear,
        impl, jubatus::util::lang::_1));
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

#endif  // JUBATUS_SERVER_SERVER_CLASSIFIER_SERVER_HPP_
