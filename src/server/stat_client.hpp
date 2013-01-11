
// This file is auto-generated from stat.idl
// *** DO NOT EDIT ***

#ifndef STAT_CLIENT_HPP_
#define STAT_CLIENT_HPP_


#include "stat_types.hpp"
#include <jubatus/msgpack/rpc/client.h>


namespace jubatus {

namespace client {

class stat {
public:
  stat(const std::string &host, uint64_t port, double timeout_sec)
    : c_(host, port) {
    c_.set_timeout( timeout_sec );
  }

  std::string get_config(std::string name) {
    return c_.call("get_config", name).get<std::string>();
  }

  bool push(std::string name, std::string key, double val) {
    return c_.call("push", name, key, val).get<bool>();
  }

  double sum(std::string name, std::string key) {
    return c_.call("sum", name, key).get<double>();
  }

  double stddev(std::string name, std::string key) {
    return c_.call("stddev", name, key).get<double>();
  }

  double max(std::string name, std::string key) {
    return c_.call("max", name, key).get<double>();
  }

  double min(std::string name, std::string key) {
    return c_.call("min", name, key).get<double>();
  }

  double entropy(std::string name, std::string key) {
    return c_.call("entropy", name, key).get<double>();
  }

  double moment(std::string name, std::string key, int32_t n, double c) {
    return c_.call("moment", name, key, n, c).get<double>();
  }

  bool save(std::string name, std::string id) {
    return c_.call("save", name, id).get<bool>();
  }

  bool load(std::string name, std::string id) {
    return c_.call("load", name, id).get<bool>();
  }

  std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
    return c_.call("get_status", name).get<std::map<std::string, std::map<std::string, std::string > > >();
  }

private:
  msgpack::rpc::client c_;
};

} // namespace client

} // namespace jubatus



#endif // STAT_CLIENT_HPP_
