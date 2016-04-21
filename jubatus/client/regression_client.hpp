// This file is auto-generated from regression.idl(0.6.4-33-gcc8d7ca) with jenerator version 0.8.5-6-g5a2c923/feature/improve-get_labels-ulong
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_REGRESSION_CLIENT_HPP_
#define JUBATUS_CLIENT_REGRESSION_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <jubatus/client/common/client.hpp>
#include <jubatus/client/common/datum.hpp>
#include "regression_types.hpp"

namespace jubatus {
namespace regression {
namespace client {

class regression : public jubatus::client::common::client {
 public:
  regression(const std::string& host, uint64_t port, const std::string& name,
      unsigned int timeout_sec)
      : client(host, port, name, timeout_sec) {
  }

  int32_t train(const std::vector<scored_datum>& train_data) {
    msgpack::rpc::future f = c_.call("train", name_, train_data);
    return f.get<int32_t>();
  }

  std::vector<float> estimate(
      const std::vector<jubatus::client::common::datum>& estimate_data) {
    msgpack::rpc::future f = c_.call("estimate", name_, estimate_data);
    return f.get<std::vector<float> >();
  }

  bool clear() {
    msgpack::rpc::future f = c_.call("clear", name_);
    return f.get<bool>();
  }
};

}  // namespace client
}  // namespace regression
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_REGRESSION_CLIENT_HPP_
