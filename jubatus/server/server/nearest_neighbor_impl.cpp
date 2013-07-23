// This file is auto-generated from nearest_neighbor.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/shared_ptr.h>

#include "../../server/framework.hpp"
#include "nearest_neighbor_server.hpp"
#include "nearest_neighbor_serv.hpp"

namespace jubatus {
namespace server {

class nearest_neighbor_impl_ : public nearest_neighbor<nearest_neighbor_impl_> {
 public:
  explicit nearest_neighbor_impl_(
      const jubatus::server::framework::server_argv& a):
    nearest_neighbor<nearest_neighbor_impl_>(a.timeout),
    p_(new jubatus::server::framework::server_helper<nearest_neighbor_serv>(a,
         true)) {
  }
  bool init_table(std::string name) {
    JWLOCK_(p_);
    return get_p()->init_table();
  }

  bool clear(std::string name) {
    JWLOCK_(p_);
    return get_p()->clear();
  }

  bool set_row(std::string name, std::string id,
       jubatus::core::fv_converter::datum d) {
    JWLOCK_(p_);
    return get_p()->set_row(id, d);
  }

  neighbor_result neighbor_row_from_id(std::string name, std::string id,
       uint32_t size) {
    JRLOCK_(p_);
    return get_p()->neighbor_row_from_id(id, size);
  }

  neighbor_result neighbor_row_from_data(std::string name,
       jubatus::core::fv_converter::datum query, uint32_t size) {
    JRLOCK_(p_);
    return get_p()->neighbor_row_from_data(query, size);
  }

  neighbor_result similar_row_from_id(std::string name, std::string id,
       int32_t ret_num) {
    JRLOCK_(p_);
    return get_p()->similar_row_from_id(id, ret_num);
  }

  neighbor_result similar_row_from_data(std::string name,
       jubatus::core::fv_converter::datum query, int32_t ret_num) {
    JRLOCK_(p_);
    return get_p()->similar_row_from_data(query, ret_num);
  }

  bool save(std::string name, std::string id) {
    NOLOCK_(p_);
    return get_p()->save(id);
  }

  bool load(std::string name, std::string id) {
    JWLOCK_(p_);
    return get_p()->load(id);
  }

  std::map<std::string, std::map<std::string, std::string> > get_status(
      std::string name) {
    JRLOCK_(p_);
    return p_->get_status();
  }
  int run() { return p_->start(*this); }
  pfi::lang::shared_ptr<nearest_neighbor_serv> get_p() { return p_->server(); }

 private:
  pfi::lang::shared_ptr<jubatus::server::framework::server_helper<nearest_neighbor_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::nearest_neighbor_impl_>
      (argc, argv, "nearest_neighbor");
}
