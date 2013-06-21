// This file is auto-generated from anomaly.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_ANOMALY_CLIENT_HPP_
#define JUBATUS_SERVER_SERVER_ANOMALY_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <jubatus/msgpack/rpc/client.h>
#include "anomaly_types.hpp"

namespace jubatus {
namespace client {

class anomaly {
 public:
  anomaly(const std::string& host, uint64_t port, double timeout_sec)
      : c_(host, port) {
    c_.set_timeout(timeout_sec);
  }

  std::string get_config(std::string name) {
    msgpack::rpc::future f = c_.call("get_config", name);
    return f.get<std::string>();
  }

  bool clear_row(std::string name, std::string id) {
    msgpack::rpc::future f = c_.call("clear_row", name, id);
    return f.get<bool>();
  }

  std::pair<std::string, float> add(std::string name, datum row) {
    msgpack::rpc::future f = c_.call("add", name, row);
    return f.get<std::pair<std::string, float> >();
  }

  float update(std::string name, std::string id, datum row) {
    msgpack::rpc::future f = c_.call("update", name, id, row);
    return f.get<float>();
  }

  bool clear(std::string name) {
    msgpack::rpc::future f = c_.call("clear", name);
    return f.get<bool>();
  }

  float calc_score(std::string name, datum row) {
    msgpack::rpc::future f = c_.call("calc_score", name, row);
    return f.get<float>();
  }

  std::vector<std::string> get_all_rows(std::string name) {
    msgpack::rpc::future f = c_.call("get_all_rows", name);
    return f.get<std::vector<std::string> >();
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
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_ANOMALY_CLIENT_HPP_
