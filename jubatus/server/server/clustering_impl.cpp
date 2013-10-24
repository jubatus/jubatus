// This file is auto-generated from clustering.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/shared_ptr.h>

#include "../../server/framework.hpp"
#include "clustering_server.hpp"
#include "clustering_serv.hpp"

namespace jubatus {
namespace server {

class clustering_impl_ : public clustering<clustering_impl_> {
 public:
  explicit clustering_impl_(const jubatus::server::framework::server_argv& a):
    clustering<clustering_impl_>(a.timeout),
    p_(new jubatus::server::framework::server_helper<clustering_serv>(a,
        false)) {
  }
  std::string get_config(const std::string& name) {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  bool push(const std::string& name,
      const std::vector<jubatus::core::fv_converter::datum>& points) {
    JWLOCK_(p_);
    return get_p()->push(points);
  }

  uint32_t get_revision(const std::string& name) {
    JRLOCK_(p_);
    return get_p()->get_revision();
  }

  std::vector<std::vector<std::pair<double,
      jubatus::core::fv_converter::datum> > > get_core_members(
      const std::string& name) {
    JRLOCK_(p_);
    return get_p()->get_core_members();
  }

  std::vector<jubatus::core::fv_converter::datum> get_k_center(
      const std::string& name) {
    JRLOCK_(p_);
    return get_p()->get_k_center();
  }

  jubatus::core::fv_converter::datum get_nearest_center(const std::string& name,
      const jubatus::core::fv_converter::datum& point) {
    JRLOCK_(p_);
    return get_p()->get_nearest_center(point);
  }

  std::vector<std::pair<double,
      jubatus::core::fv_converter::datum> > get_nearest_members(
      const std::string& name,
      const jubatus::core::fv_converter::datum& point) {
    JRLOCK_(p_);
    return get_p()->get_nearest_members(point);
  }

  bool save(const std::string& name, const std::string& id) {
    JWLOCK_(p_);
    return get_p()->save(id);
  }

  bool load(const std::string& name, const std::string& id) {
    JWLOCK_(p_);
    return get_p()->load(id);
  }

  std::map<std::string, std::map<std::string, std::string> > get_status(
      const std::string& name) {
    JRLOCK_(p_);
    return p_->get_status();
  }
  int run() { return p_->start(*this); }
  pfi::lang::shared_ptr<clustering_serv> get_p() { return p_->server(); }

 private:
  pfi::lang::shared_ptr<jubatus::server::framework::server_helper<clustering_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::clustering_impl_>
      (argc, argv, "clustering");
}
