// This file is auto-generated from anomaly.idl(1.0.6-6-g2cf96c3) with jenerator version 0.9.4-42-g70f7539/develop
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_ANOMALY_CLIENT_HPP_
#define JUBATUS_CLIENT_ANOMALY_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <jubatus/client/common/client.hpp>
#include <jubatus/client/common/datum.hpp>
#include "anomaly_types.hpp"

namespace jubatus {
namespace anomaly {
namespace client {

class anomaly : public jubatus::client::common::client {
 public:
  anomaly(const std::string& host, uint64_t port, const std::string& name,
      unsigned int timeout_sec)
      : client(host, port, name, timeout_sec) {
  }

  bool clear_row(const std::string& id) {
    msgpack::rpc::future f = c_.call("clear_row", name_, id);
    return f.get<bool>();
  }

  id_with_score add(const jubatus::client::common::datum& row) {
    msgpack::rpc::future f = c_.call("add", name_, row);
    return f.get<id_with_score>();
  }

  std::vector<std::string> add_bulk(
      const std::vector<jubatus::client::common::datum>& data) {
    msgpack::rpc::future f = c_.call("add_bulk", name_, data);
    return f.get<std::vector<std::string> >();
  }

  float update(const std::string& id,
      const jubatus::client::common::datum& row) {
    msgpack::rpc::future f = c_.call("update", name_, id, row);
    return f.get<float>();
  }

  float overwrite(const std::string& id,
      const jubatus::client::common::datum& row) {
    msgpack::rpc::future f = c_.call("overwrite", name_, id, row);
    return f.get<float>();
  }

  bool clear() {
    msgpack::rpc::future f = c_.call("clear", name_);
    return f.get<bool>();
  }

  float calc_score(const jubatus::client::common::datum& row) {
    msgpack::rpc::future f = c_.call("calc_score", name_, row);
    return f.get<float>();
  }

  std::vector<std::string> get_all_rows() {
    msgpack::rpc::future f = c_.call("get_all_rows", name_);
    return f.get<std::vector<std::string> >();
  }
};

}  // namespace client
}  // namespace anomaly
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_ANOMALY_CLIENT_HPP_
