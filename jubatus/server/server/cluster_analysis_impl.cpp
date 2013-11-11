// This file is auto-generated from cluster_analysis.idl(0.4.5-347-gbd3e713) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/shared_ptr.h>

#include "../../server/framework.hpp"
#include "cluster_analysis_serv.hpp"

namespace jubatus {
namespace server {

class cluster_analysis_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit cluster_analysis_impl(
      const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<cluster_analysis_serv>(a,
        false)) {

    rpc_server::add<bool(std::string, std::string)>("add_snapshot",
        pfi::lang::bind(&cluster_analysis_impl::add_snapshot, this,
        pfi::lang::_2));
    rpc_server::add<std::vector<jubatus::core::cluster_analysis::change_graph>(
        std::string)>("get_history", pfi::lang::bind(
        &cluster_analysis_impl::get_history, this));
    rpc_server::add<std::vector<jubatus::core::cluster_analysis::clustering_snapshot>(std::string)>("get_snapshots", pfi::lang::bind(&cluster_analysis_impl::get_snapshots, this));

    rpc_server::add<std::string(std::string)>("get_config", pfi::lang::bind(
        &cluster_analysis_impl::get_config, this));
    rpc_server::add<bool(std::string, std::string)>("save", pfi::lang::bind(
        &cluster_analysis_impl::save, this, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load", pfi::lang::bind(
        &cluster_analysis_impl::load, this, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", pfi::lang::bind(
        &cluster_analysis_impl::get_status, this));
  }

  bool add_snapshot(const std::string& clustering_name) {
    JWLOCK_(p_);
    return get_p()->add_snapshot(clustering_name);
  }

  std::vector<jubatus::core::cluster_analysis::change_graph> get_history() {
    JRLOCK_(p_);
    return get_p()->get_history();
  }

  std::vector<jubatus::core::cluster_analysis::clustering_snapshot> get_snapshots() {
    JRLOCK_(p_);
    return get_p()->get_snapshots();
  }

  std::string get_config() {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  bool save(const std::string& id) {
    JWLOCK_(p_);
    return get_p()->save(id);
  }

  bool load(const std::string& id) {
    JWLOCK_(p_);
    return get_p()->load(id);
  }

  std::map<std::string, std::map<std::string, std::string> > get_status() {
    JRLOCK_(p_);
    return p_->get_status();
  }

  int run() { return p_->start(*this); }
  pfi::lang::shared_ptr<cluster_analysis_serv> get_p() { return p_->server(); }

 private:
  pfi::lang::shared_ptr<jubatus::server::framework::server_helper<cluster_analysis_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::cluster_analysis_impl>
      (argc, argv, "cluster_analysis");
}
