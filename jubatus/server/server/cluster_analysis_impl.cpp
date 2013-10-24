// This file is auto-generated from cluster_analysis.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/shared_ptr.h>

#include "../../server/framework.hpp"
#include "cluster_analysis_server.hpp"
#include "cluster_analysis_serv.hpp"

namespace jubatus {
namespace server {

class cluster_analysis_impl_ : public cluster_analysis<cluster_analysis_impl_> {
 public:
  explicit cluster_analysis_impl_(
      const jubatus::server::framework::server_argv& a):
    cluster_analysis<cluster_analysis_impl_>(a.timeout),
    p_(new jubatus::server::framework::server_helper<cluster_analysis_serv>(a,
        false)) {
  }
  std::string get_config(const std::string& name) {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  bool add_snapshot(const std::string& name,
      const std::string& clustering_name) {
    JWLOCK_(p_);
    return get_p()->add_snapshot(clustering_name);
  }

  std::vector<jubatus::core::cluster_analysis::change_graph> get_history(
      const std::string& name) {
    JRLOCK_(p_);
    return get_p()->get_history();
  }

  std::vector<jubatus::core::cluster_analysis::clustering_snapshot> get_snapshots(const std::string& name) {
    JRLOCK_(p_);
    return get_p()->get_snapshots();
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
    jubatus::server::framework::run_server<jubatus::server::cluster_analysis_impl_>
      (argc, argv, "cluster_analysis");
}
