// This file is auto-generated from stat.idl(0.4.5-331-gd7b15b4) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_STAT_CLIENT_HPP_
#define JUBATUS_CLIENT_STAT_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <jubatus/client/common/client.hpp>
#include <jubatus/client/common/datum.hpp>
#include "stat_types.hpp"

namespace jubatus {
namespace stat {
namespace client {

class stat : public jubatus::client::common::client {
 public:
  stat(const std::string& host, uint64_t port, const std::string& name,
      unsigned int timeout_sec)
      : client(host, port, name, timeout_sec) {
  }

  bool push(const std::string& key, double value) {
    msgpack::rpc::future f = c_.call("push", name_, key, value);
    return f.get<bool>();
  }

  double sum(const std::string& key) {
    msgpack::rpc::future f = c_.call("sum", name_, key);
    return f.get<double>();
  }

  double stddev(const std::string& key) {
    msgpack::rpc::future f = c_.call("stddev", name_, key);
    return f.get<double>();
  }

  double max(const std::string& key) {
    msgpack::rpc::future f = c_.call("max", name_, key);
    return f.get<double>();
  }

  double min(const std::string& key) {
    msgpack::rpc::future f = c_.call("min", name_, key);
    return f.get<double>();
  }

  double entropy(const std::string& key) {
    msgpack::rpc::future f = c_.call("entropy", name_, key);
    return f.get<double>();
  }

  double moment(const std::string& key, int32_t degree, double center) {
    msgpack::rpc::future f = c_.call("moment", name_, key, degree, center);
    return f.get<double>();
  }

  bool clear() {
    msgpack::rpc::future f = c_.call("clear", name_);
    return f.get<bool>();
  }
};

}  // namespace client
}  // namespace stat
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_STAT_CLIENT_HPP_
