// This file is auto-generated from stat.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_STAT_CLIENT_HPP_
#define JUBATUS_CLIENT_STAT_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <jubatus/msgpack/rpc/client.h>
#include "stat_types.hpp"

namespace jubatus {
namespace stat {
namespace client {

class stat {
 public:
  stat(const std::string& host, uint64_t port, double timeout_sec)
      : c_(host, port) {
    c_.set_timeout(timeout_sec);
  }

  std::string get_config(std::string name) {
    msgpack::rpc::future f = c_.call("get_config", name);
    return f.get<std::string>();
  }

  bool push(std::string name, std::string key, double value) {
    msgpack::rpc::future f = c_.call("push", name, key, value);
    return f.get<bool>();
  }

  double sum(std::string name, std::string key) {
    msgpack::rpc::future f = c_.call("sum", name, key);
    return f.get<double>();
  }

  double stddev(std::string name, std::string key) {
    msgpack::rpc::future f = c_.call("stddev", name, key);
    return f.get<double>();
  }

  double max(std::string name, std::string key) {
    msgpack::rpc::future f = c_.call("max", name, key);
    return f.get<double>();
  }

  double min(std::string name, std::string key) {
    msgpack::rpc::future f = c_.call("min", name, key);
    return f.get<double>();
  }

  double entropy(std::string name, std::string key) {
    msgpack::rpc::future f = c_.call("entropy", name, key);
    return f.get<double>();
  }

  double moment(std::string name, std::string key, int32_t degree,
       double center) {
    msgpack::rpc::future f = c_.call("moment", name, key, degree, center);
    return f.get<double>();
  }

  bool clear(std::string name) {
    msgpack::rpc::future f = c_.call("clear", name);
    return f.get<bool>();
  }

  bool save(std::string name, std::string id) {
    msgpack::rpc::future f = c_.call("save", name, id);
    return f.get<bool>();
  }

  bool load(std::string name, std::string id) {
    msgpack::rpc::future f = c_.call("load", name, id);
    return f.get<bool>();
  }

  std::map<std::string, std::map<std::string, std::string> > get_status(
      std::string name) {
    msgpack::rpc::future f = c_.call("get_status", name);
    return f.get<std::map<std::string, std::map<std::string, std::string> > >();
  }

  msgpack::rpc::client& get_client() {
    return c_;
  }

 private:
  msgpack::rpc::client c_;
};

}  // namespace client
}  // namespace stat
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_STAT_CLIENT_HPP_
