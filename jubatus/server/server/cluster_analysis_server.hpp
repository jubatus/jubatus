// This file is auto-generated from cluster_analysis.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_CLUSTER_ANALYSIS_SERVER_HPP_
#define JUBATUS_SERVER_SERVER_CLUSTER_ANALYSIS_SERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/bind.h>

#include "../../server/common/mprpc/rpc_server.hpp"
#include "cluster_analysis_types.hpp"

namespace jubatus {
namespace server {

template <class Impl>
class cluster_analysis : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit cluster_analysis(double timeout_sec) : rpc_server(timeout_sec) {
    Impl* impl = static_cast<Impl*>(this);
    rpc_server::add<std::string(std::string)>("get_config", pfi::lang::bind(
        &Impl::get_config, impl, pfi::lang::_1));
    rpc_server::add<bool(std::string, std::string)>("add_snapshot",
        pfi::lang::bind(&Impl::add_snapshot, impl, pfi::lang::_1,
        pfi::lang::_2));
    rpc_server::add<std::vector<jubatus::core::cluster_analysis::change_graph>(
        std::string)>("get_history", pfi::lang::bind(&Impl::get_history, impl,
        pfi::lang::_1));
    rpc_server::add<std::vector<jubatus::core::cluster_analysis::clustering_snapshot>(std::string)>("get_snapshots", pfi::lang::bind(&Impl::get_snapshots, impl, pfi::lang::_1));
  }
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_CLUSTER_ANALYSIS_SERVER_HPP_
