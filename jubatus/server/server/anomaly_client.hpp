// This file is auto-generated from anomaly.idl(0.4.5-331-gd7b15b4) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_ANOMALY_CLIENT_HPP_
#define JUBATUS_SERVER_SERVER_ANOMALY_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <jubatus/client/common/client.hpp>
#include "jubatus/core/fv_converter/datum.hpp"
#include "anomaly_types.hpp"

namespace jubatus {
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

  id_with_score add(const jubatus::core::fv_converter::datum& row) {
    msgpack::rpc::future f = c_.call("add", name_, row);
    return f.get<id_with_score>();
  }

  float update(const std::string& id,
      const jubatus::core::fv_converter::datum& row) {
    msgpack::rpc::future f = c_.call("update", name_, id, row);
    return f.get<float>();
  }

  float overwrite(const std::string& id,
      const jubatus::core::fv_converter::datum& row) {
    msgpack::rpc::future f = c_.call("overwrite", name_, id, row);
    return f.get<float>();
  }

  bool clear() {
    msgpack::rpc::future f = c_.call("clear", name_);
    return f.get<bool>();
  }

  float calc_score(const jubatus::core::fv_converter::datum& row) {
    msgpack::rpc::future f = c_.call("calc_score", name_, row);
    return f.get<float>();
  }

  std::vector<std::string> get_all_rows() {
    msgpack::rpc::future f = c_.call("get_all_rows", name_);
    return f.get<std::vector<std::string> >();
  }
};

}  // namespace client
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_ANOMALY_CLIENT_HPP_
