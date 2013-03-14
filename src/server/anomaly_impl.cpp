// This file is auto-generated from anomaly.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "../framework.hpp"
#include "anomaly_server.hpp"
#include "anomaly_serv.hpp"

namespace jubatus {
namespace server {

class anomaly_impl_ : public anomaly<anomaly_impl_> {
 public:
  explicit anomaly_impl_(const jubatus::framework::server_argv& a):
    anomaly<anomaly_impl_>(a.timeout),
    p_(new jubatus::framework::server_helper<anomaly_serv>(a, true)) {
  }
  std::string get_config(std::string name) {
    JRLOCK__(p_);
    return get_p()->get_config();
  }

  bool clear_row(std::string name, std::string id) {
    JWLOCK__(p_);
    return get_p()->clear_row(id);
  }

  std::pair<std::string, float> add(std::string name, datum row) {
    JWLOCK__(p_);
    return get_p()->add(row);
  }

  float update(std::string name, std::string id, datum row) {
    JWLOCK__(p_);
    return get_p()->update(id, row);
  }

  bool clear(std::string name) {
    JWLOCK__(p_);
    return get_p()->clear();
  }

  float calc_score(std::string name, datum row) {
    JRLOCK__(p_);
    return get_p()->calc_score(row);
  }

  std::vector<std::string> get_all_rows(std::string name) {
    JRLOCK__(p_);
    return get_p()->get_all_rows();
  }

  bool save(std::string name, std::string id) {
    JWLOCK__(p_);
    return get_p()->save(id);
  }

  bool load(std::string name, std::string id) {
    JWLOCK__(p_);
    return get_p()->load(id);
  }

  std::map<std::string, std::map<std::string, std::string> > get_status(
      std::string name) {
    JRLOCK__(p_);
    return p_->get_status();
  }
  int run() { return p_->start(*this); }
  common::cshared_ptr<anomaly_serv> get_p() { return p_->server(); }

 private:
  common::cshared_ptr<jubatus::framework::server_helper<anomaly_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::framework::run_server<jubatus::server::anomaly_impl_>
      (argc, argv, "anomaly");
}
