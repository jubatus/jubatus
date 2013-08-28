// This file is auto-generated from regression.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/shared_ptr.h>

#include "../../server/framework.hpp"
#include "regression_server.hpp"
#include "regression_serv.hpp"

namespace jubatus {
namespace server {

class regression_impl_ : public regression<regression_impl_> {
 public:
  explicit regression_impl_(const jubatus::server::framework::server_argv& a):
    regression<regression_impl_>(a.timeout),
    p_(new jubatus::server::framework::server_helper<regression_serv>(a,
         false)) {
  }
  std::string get_config(const std::string& name) {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  int32_t train(const std::string& name, const std::vector<std::pair<float,
       jubatus::core::fv_converter::datum> >& train_data) {
    JWLOCK_(p_);
    return get_p()->train(train_data);
  }

  std::vector<float> estimate(const std::string& name,
       const std::vector<jubatus::core::fv_converter::datum>& estimate_data) {
    JRLOCK_(p_);
    return get_p()->estimate(estimate_data);
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
  pfi::lang::shared_ptr<regression_serv> get_p() { return p_->server(); }

 private:
  pfi::lang::shared_ptr<jubatus::server::framework::server_helper<regression_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::regression_impl_>
      (argc, argv, "regression");
}
