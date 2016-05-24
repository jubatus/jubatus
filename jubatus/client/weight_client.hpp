// This file is auto-generated from weight.idl with jenerator version 0.8.5-6-g5a2c923/develop
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_WEIGHT_CLIENT_HPP_
#define JUBATUS_CLIENT_WEIGHT_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <jubatus/client/common/client.hpp>
#include <jubatus/client/common/datum.hpp>
#include "weight_types.hpp"

namespace jubatus {
namespace weight {
namespace client {

class weight : public jubatus::client::common::client {
 public:
  weight(const std::string& host, uint64_t port, const std::string& name,
      unsigned int timeout_sec)
      : client(host, port, name, timeout_sec) {
  }

  std::vector<feature> update(const jubatus::client::common::datum& d) {
    msgpack::rpc::future f = c_.call("update", name_, d);
    return f.get<std::vector<feature> >();
  }

  std::vector<feature> calc_weight(const jubatus::client::common::datum& d) {
    msgpack::rpc::future f = c_.call("calc_weight", name_, d);
    return f.get<std::vector<feature> >();
  }

  bool clear() {
    msgpack::rpc::future f = c_.call("clear", name_);
    return f.get<bool>();
  }
};

}  // namespace client
}  // namespace weight
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_WEIGHT_CLIENT_HPP_
