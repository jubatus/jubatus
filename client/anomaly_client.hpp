// This file is auto-generated from anomaly.idl
// *** DO NOT EDIT ***

#ifndef ANOMALY_CLIENT_HPP_
#define ANOMALY_CLIENT_HPP_

#include "anomaly_types.hpp"
#include <jubatus/msgpack/rpc/client.h>

namespace jubatus {

namespace anomaly {

namespace client {

class anomaly {
 public:
  anomaly(const std::string &host, uint64_t port, double timeout_sec)
      : c_(host, port) {
    c_.set_timeout(timeout_sec);
  }

  std::string get_config(std::string name) {
    return c_.call("get_config", name).get<std::string>();
  }

  bool clear_row(std::string name, std::string id) {
    return c_.call("clear_row", name, id).get<bool>();
  }

  std::pair<std::string, float> add(std::string name, datum row) {
    return c_.call("add", name, row).get<std::pair<std::string, float> >();
  }

  float update(std::string name, std::string id, datum row) {
    return c_.call("update", name, id, row).get<float>();
  }

  bool clear(std::string name) {
    return c_.call("clear", name).get<bool>();
  }

  float calc_score(std::string name, datum row) {
    return c_.call("calc_score", name, row).get<float>();
  }

  std::vector<std::string> get_all_rows(std::string name) {
    return c_.call("get_all_rows", name).get<std::vector<std::string> >();
  }

  bool save(std::string name, std::string id) {
    return c_.call("save", name, id).get<bool>();
  }

  bool load(std::string name, std::string id) {
    return c_.call("load", name, id).get<bool>();
  }

  std::map<std::string, std::map<std::string, std::string> > get_status(
      std::string name) {
    return c_.call("get_status", name)
        .get<std::map<std::string, std::map<std::string, std::string> > >();
  }

 private:
  msgpack::rpc::client c_;
};

}  // namespace client

}  // namespace anomaly

}  // namespace jubatus

#endif // ANOMALY_CLIENT_HPP_
