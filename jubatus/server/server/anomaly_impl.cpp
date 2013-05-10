// This file is auto-generated from anomaly.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/shared_ptr.h>

#include "../framework.hpp"
#include "anomaly_server.hpp"
#include "anomaly_serv.hpp"

namespace jubatus {
namespace server {

class anomaly_impl_ : public anomaly<anomaly_impl_> {
 public:
  explicit anomaly_impl_(const jubatus::server::framework::server_argv& a):
    anomaly<anomaly_impl_>(a.timeout),
    p_(new jubatus::server::framework::server_helper<anomaly_serv>(a, true)) {
  }
  std::string get_config(std::string name) {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  bool clear_row(std::string name, std::string id) {
    JWLOCK_(p_);
    return get_p()->clear_row(id);
  }

  std::pair<std::string, float> add(std::string name, datum row) {
    NOLOCK_(p_);
    return get_p()->add(row);
  }

  float update(std::string name, std::string id, datum row) {
    JWLOCK_(p_);
    return get_p()->update(id, row);
  }

  bool clear(std::string name) {
    JWLOCK_(p_);
    return get_p()->clear();
  }

  float calc_score(std::string name, datum row) {
    JRLOCK_(p_);
    return get_p()->calc_score(row);
  }

  std::vector<std::string> get_all_rows(std::string name) {
    JRLOCK_(p_);
    return get_p()->get_all_rows();
  }

  bool save(std::string name, std::string id) {
    JWLOCK_(p_);
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
  pfi::lang::shared_ptr<anomaly_serv> get_p() { return p_->server(); }

 private:
  pfi::lang::shared_ptr<jubatus::server::framework::server_helper<anomaly_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::anomaly_impl_>
      (argc, argv, "anomaly");
}
