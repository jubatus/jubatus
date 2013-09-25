// This file is auto-generated from window_classifier.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "../framework.hpp"
#include "window_classifier_server.hpp"
#include "window_classifier_serv.hpp"

namespace jubatus {
namespace server {

class window_classifier_impl_ : public window_classifier<window_classifier_impl_> {
 public:
  explicit window_classifier_impl_(const jubatus::framework::server_argv& a):
    window_classifier<window_classifier_impl_>(a.timeout),
    p_(new jubatus::framework::server_helper<window_classifier_serv>(a, true)) {
  }
  std::string get_config(std::string name) {
    JRLOCK__(p_);
    return get_p()->get_config();
  }
  
  int32_t train(std::string name, std::string window_id,
       std::vector<std::pair<std::string, datum> > data) {
    JWLOCK__(p_);
    return get_p()->train(window_id, data);
  }
  
  std::vector<estimate_result> classify(std::string name, std::string window_id,
       datum data) {
    JRLOCK__(p_);
    return get_p()->classify(window_id, data);
  }
  
  int32_t push(std::string name, std::string window_id,
       std::vector<datum> data) {
    JWLOCK__(p_);
    return get_p()->push(window_id, data);
  }
  
  bool clear_window(std::string name, std::string window_id) {
    JWLOCK__(p_);
    return get_p()->clear_window(window_id);
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
  common::cshared_ptr<window_classifier_serv> get_p() { return p_->server(); }

 private:
  common::cshared_ptr<jubatus::framework::server_helper<window_classifier_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::framework::run_server<jubatus::server::window_classifier_impl_>
      (argc, argv, "window_classifier");
}
