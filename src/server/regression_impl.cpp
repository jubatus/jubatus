// This file is auto-generated from regression.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "../framework.hpp"
#include "regression_server.hpp"
#include "regression_serv.hpp"

namespace jubatus {
namespace server {

class regression_impl_ : public regression<regression_impl_> {
 public:
  explicit regression_impl_(const jubatus::framework::server_argv& a):
    regression<regression_impl_>(a.timeout),
    p_(new jubatus::framework::server_helper<regression_serv>(a, false)) {
  }
  std::string get_config(std::string name) {
    JRLOCK__(p_);
    return get_p()->get_config();
  }
  
  int32_t train(std::string name, std::vector<std::pair<float,
       datum> > train_data) {
    JWLOCK__(p_);
    return get_p()->train(train_data);
  }
  
  std::vector<float> estimate(std::string name,
       std::vector<datum> estimate_data) {
    JRLOCK__(p_);
    return get_p()->estimate(estimate_data);
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
  common::cshared_ptr<regression_serv> get_p() { return p_->server(); }

 private:
  common::cshared_ptr<jubatus::framework::server_helper<regression_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int args, char** argv) {
  return
    jubatus::framework::run_server<jubatus::server::regression_impl_>
      (args, argv, "regression");
}
