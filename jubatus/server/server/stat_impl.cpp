// This file is auto-generated from stat.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/shared_ptr.h>

#include "../../server/framework.hpp"
#include "stat_server.hpp"
#include "stat_serv.hpp"

namespace jubatus {
namespace server {

class stat_impl_ : public stat<stat_impl_> {
 public:
  explicit stat_impl_(const jubatus::server::framework::server_argv& a):
    stat<stat_impl_>(a.timeout),
    p_(new jubatus::server::framework::server_helper<stat_serv>(a, true)) {
  }
  std::string get_config(const std::string& name) {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  bool push(const std::string& name, const std::string& key, double value) {
    JWLOCK_(p_);
    return get_p()->push(key, value);
  }

  double sum(const std::string& name, const std::string& key) {
    JRLOCK_(p_);
    return get_p()->sum(key);
  }

  double stddev(const std::string& name, const std::string& key) {
    JRLOCK_(p_);
    return get_p()->stddev(key);
  }

  double max(const std::string& name, const std::string& key) {
    JRLOCK_(p_);
    return get_p()->max(key);
  }

  double min(const std::string& name, const std::string& key) {
    JRLOCK_(p_);
    return get_p()->min(key);
  }

  double entropy(const std::string& name, const std::string& key) {
    JRLOCK_(p_);
    return get_p()->entropy(key);
  }

  double moment(const std::string& name, const std::string& key, int32_t degree,
      double center) {
    JRLOCK_(p_);
    return get_p()->moment(key, degree, center);
  }

  bool clear(const std::string& name) {
    JWLOCK_(p_);
    return get_p()->clear();
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
  pfi::lang::shared_ptr<stat_serv> get_p() { return p_->server(); }

 private:
  pfi::lang::shared_ptr<jubatus::server::framework::server_helper<stat_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::stat_impl_>
      (argc, argv, "stat");
}
