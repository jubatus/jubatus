// This file is auto-generated from cluster_analysis.idl(0.4.5-347-gbd3e713) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_CLUSTER_ANALYSIS_CLIENT_HPP_
#define JUBATUS_CLIENT_CLUSTER_ANALYSIS_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <jubatus/client/common/client.hpp>
#include <jubatus/client/common/datum.hpp>
#include "cluster_analysis_types.hpp"

namespace jubatus {
namespace cluster_analysis {
namespace client {

class cluster_analysis : public jubatus::client::common::client {
 public:
  cluster_analysis(const std::string& host, uint64_t port,
      const std::string& name, unsigned int timeout_sec)
      : client(host, port, name, timeout_sec) {
  }

  bool add_snapshot(const std::string& clustering_name) {
    msgpack::rpc::future f = c_.call("add_snapshot", name_, clustering_name);
    return f.get<bool>();
  }

  std::vector<change_graph> get_history() {
    msgpack::rpc::future f = c_.call("get_history", name_);
    return f.get<std::vector<change_graph> >();
  }

  std::vector<clustering_snapshot> get_snapshots() {
    msgpack::rpc::future f = c_.call("get_snapshots", name_);
    return f.get<std::vector<clustering_snapshot> >();
  }
};

}  // namespace client
}  // namespace cluster_analysis
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_CLUSTER_ANALYSIS_CLIENT_HPP_
