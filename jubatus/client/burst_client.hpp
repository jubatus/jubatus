// This file is auto-generated from burst.idl(0.6.1-24-g3a65019) with jenerator version 0.5.4-224-g49229fa/feature/burst
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_BURST_CLIENT_HPP_
#define JUBATUS_CLIENT_BURST_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <jubatus/client/common/client.hpp>
#include <jubatus/client/common/datum.hpp>
#include "burst_types.hpp"

namespace jubatus {
namespace burst {
namespace client {

class burst : public jubatus::client::common::client {
 public:
  burst(const std::string& host, uint64_t port, const std::string& name,
      unsigned int timeout_sec)
      : client(host, port, name, timeout_sec) {
  }

  int32_t add_documents(const std::vector<st_document>& data) {
    msgpack::rpc::future f = c_.call("add_documents", name_, data);
    return f.get<int32_t>();
  }

  st_window get_result(const std::string& keyword) {
    msgpack::rpc::future f = c_.call("get_result", name_, keyword);
    return f.get<st_window>();
  }

  st_window get_result_at(const std::string& keyword, double pos) {
    msgpack::rpc::future f = c_.call("get_result_at", name_, keyword, pos);
    return f.get<st_window>();
  }

  std::map<std::string, st_window> get_all_bursted_results() {
    msgpack::rpc::future f = c_.call("get_all_bursted_results", name_);
    return f.get<std::map<std::string, st_window> >();
  }

  std::map<std::string, st_window> get_all_bursted_results_at(double pos) {
    msgpack::rpc::future f = c_.call("get_all_bursted_results_at", name_, pos);
    return f.get<std::map<std::string, st_window> >();
  }

  std::vector<st_keyword> get_all_keywords() {
    msgpack::rpc::future f = c_.call("get_all_keywords", name_);
    return f.get<std::vector<st_keyword> >();
  }

  bool add_keyword(const st_keyword& keyword) {
    msgpack::rpc::future f = c_.call("add_keyword", name_, keyword);
    return f.get<bool>();
  }

  bool remove_keyword(const std::string& keyword) {
    msgpack::rpc::future f = c_.call("remove_keyword", name_, keyword);
    return f.get<bool>();
  }

  bool remove_all_keywords() {
    msgpack::rpc::future f = c_.call("remove_all_keywords", name_);
    return f.get<bool>();
  }
};

}  // namespace client
}  // namespace burst
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_BURST_CLIENT_HPP_
