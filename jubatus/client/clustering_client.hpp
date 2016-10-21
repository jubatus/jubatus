// This file is auto-generated from clustering.idl(0.9.4-19-gc665909) with jenerator version 0.8.5-6-g5a2c923/feature/refactoring_clustering_api
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_CLUSTERING_CLIENT_HPP_
#define JUBATUS_CLIENT_CLUSTERING_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <jubatus/client/common/client.hpp>
#include <jubatus/client/common/datum.hpp>
#include "clustering_types.hpp"

namespace jubatus {
namespace clustering {
namespace client {

class clustering : public jubatus::client::common::client {
 public:
  clustering(const std::string& host, uint64_t port, const std::string& name,
      unsigned int timeout_sec)
      : client(host, port, name, timeout_sec) {
  }

  bool push(const std::vector<indexed_point>& points) {
    msgpack::rpc::future f = c_.call("push", name_, points);
    return f.get<bool>();
  }

  uint32_t get_revision() {
    msgpack::rpc::future f = c_.call("get_revision", name_);
    return f.get<uint32_t>();
  }

  std::vector<std::vector<weighted_datum> > get_core_members() {
    msgpack::rpc::future f = c_.call("get_core_members", name_);
    return f.get<std::vector<std::vector<weighted_datum> > >();
  }

  std::vector<std::vector<weighted_index> > get_core_members_light() {
    msgpack::rpc::future f = c_.call("get_core_members_light", name_);
    return f.get<std::vector<std::vector<weighted_index> > >();
  }

  std::vector<jubatus::client::common::datum> get_k_center() {
    msgpack::rpc::future f = c_.call("get_k_center", name_);
    return f.get<std::vector<jubatus::client::common::datum> >();
  }

  jubatus::client::common::datum get_nearest_center(
      const jubatus::client::common::datum& point) {
    msgpack::rpc::future f = c_.call("get_nearest_center", name_, point);
    return f.get<jubatus::client::common::datum>();
  }

  std::vector<weighted_datum> get_nearest_members(
      const jubatus::client::common::datum& point) {
    msgpack::rpc::future f = c_.call("get_nearest_members", name_, point);
    return f.get<std::vector<weighted_datum> >();
  }

  std::vector<weighted_index> get_nearest_members_light(
      const jubatus::client::common::datum& point) {
    msgpack::rpc::future f = c_.call("get_nearest_members_light", name_, point);
    return f.get<std::vector<weighted_index> >();
  }

  bool clear() {
    msgpack::rpc::future f = c_.call("clear", name_);
    return f.get<bool>();
  }
};

}  // namespace client
}  // namespace clustering
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_CLUSTERING_CLIENT_HPP_
