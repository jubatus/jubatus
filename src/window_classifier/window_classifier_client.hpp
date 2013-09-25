// This file is auto-generated from window_classifier.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_WINDOW_CLASSIFIER_CLIENT_HPP_
#define JUBATUS_SERVER_WINDOW_CLASSIFIER_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <jubatus/msgpack/rpc/client.h>
#include "window_classifier_types.hpp"

namespace jubatus {
namespace client {

class window_classifier {
 public:
  window_classifier(const std::string& host, uint64_t port, double timeout_sec)
      : c_(host, port) {
    c_.set_timeout(timeout_sec);
  }
  
  std::string get_config(std::string name) {
    msgpack::rpc::future f = c_.call("get_config", name);
    return f.get<std::string>();
  }
  
  int32_t train(std::string name, std::string window_id,
       std::vector<std::pair<std::string, datum> > data) {
    msgpack::rpc::future f = c_.call("train", name, window_id, data);
    return f.get<int32_t>();
  }
  
  std::vector<estimate_result> classify(std::string name, std::string window_id,
       datum data) {
    msgpack::rpc::future f = c_.call("classify", name, window_id, data);
    return f.get<std::vector<estimate_result> >();
  }
  
  int32_t push(std::string name, std::string window_id,
       std::vector<datum> data) {
    msgpack::rpc::future f = c_.call("push", name, window_id, data);
    return f.get<int32_t>();
  }
  
  bool clear_window(std::string name, std::string window_id) {
    msgpack::rpc::future f = c_.call("clear_window", name, window_id);
    return f.get<bool>();
  }
  
  bool save(std::string name, std::string id) {
    msgpack::rpc::future f = c_.call("save", name, id);
    return f.get<bool>();
  }
  
  bool load(std::string name, std::string id) {
    msgpack::rpc::future f = c_.call("load", name, id);
    return f.get<bool>();
  }
  
  std::map<std::string, std::map<std::string, std::string> > get_status(
      std::string name) {
    msgpack::rpc::future f = c_.call("get_status", name);
    return f.get<std::map<std::string, std::map<std::string, std::string> > >();
  }

 private:
  msgpack::rpc::client c_;
};

}  // namespace client
}  // namespace jubatus

#endif  // JUBATUS_SERVER_WINDOW_CLASSIFIER_CLIENT_HPP_
