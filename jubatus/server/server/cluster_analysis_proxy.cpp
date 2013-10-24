// This file is auto-generated from cluster_analysis.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "../../server/framework/aggregators.hpp"
#include "../../server/framework/proxy.hpp"
#include "cluster_analysis_types.hpp"

namespace jubatus {

int run_proxy(int argc, char* argv[]) {
  try {
    jubatus::server::framework::proxy k(
        jubatus::server::framework::proxy_argv(argc, argv, "cluster_analysis"));
    k.register_async_random<std::string>("get_config");
    k.register_async_broadcast<bool, std::string>("add_snapshot",
        pfi::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_random<std::vector<jubatus::core::cluster_analysis::change_graph> >("get_history");
    k.register_async_random<std::vector<jubatus::core::cluster_analysis::clustering_snapshot> >("get_snapshots");
    return k.run();
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    LOG(FATAL) << e.diagnostic_information(true);
    return -1;
  }
}

}  // namespace jubatus

int main(int argc, char* argv[]) {
  jubatus::run_proxy(argc, argv);
}
