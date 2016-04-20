// This file is auto-generated from bandit.idl(0.7.2-79-g2db27d7) with jenerator version 0.8.5-6-g5a2c923/feature/improve-get_labels-ulong
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_BANDIT_CLIENT_HPP_
#define JUBATUS_CLIENT_BANDIT_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <jubatus/client/common/client.hpp>
#include <jubatus/client/common/datum.hpp>
#include "bandit_types.hpp"

namespace jubatus {
namespace bandit {
namespace client {

class bandit : public jubatus::client::common::client {
 public:
  bandit(const std::string& host, uint64_t port, const std::string& name,
      unsigned int timeout_sec)
      : client(host, port, name, timeout_sec) {
  }

  bool register_arm(const std::string& arm_id) {
    msgpack::rpc::future f = c_.call("register_arm", name_, arm_id);
    return f.get<bool>();
  }

  bool delete_arm(const std::string& arm_id) {
    msgpack::rpc::future f = c_.call("delete_arm", name_, arm_id);
    return f.get<bool>();
  }

  std::string select_arm(const std::string& player_id) {
    msgpack::rpc::future f = c_.call("select_arm", name_, player_id);
    return f.get<std::string>();
  }

  bool register_reward(const std::string& player_id, const std::string& arm_id,
      double reward) {
    msgpack::rpc::future f = c_.call("register_reward", name_, player_id,
        arm_id, reward);
    return f.get<bool>();
  }

  std::map<std::string, arm_info> get_arm_info(const std::string& player_id) {
    msgpack::rpc::future f = c_.call("get_arm_info", name_, player_id);
    return f.get<std::map<std::string, arm_info> >();
  }

  bool reset(const std::string& player_id) {
    msgpack::rpc::future f = c_.call("reset", name_, player_id);
    return f.get<bool>();
  }

  bool clear() {
    msgpack::rpc::future f = c_.call("clear", name_);
    return f.get<bool>();
  }
};

}  // namespace client
}  // namespace bandit
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_BANDIT_CLIENT_HPP_
