// This file is auto-generated from classifier.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_CLASSIFIER_CLIENT_HPP_
#define JUBATUS_CLIENT_CLASSIFIER_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <jubatus/msgpack/rpc/client.h>
#include "classifier_types.hpp"

namespace jubatus {
namespace classifier {
namespace client {

class classifier {
 public:
  classifier(const std::string& host, uint64_t port, double timeout_sec)
      : c_(host, port) {
    c_.set_timeout(timeout_sec);
  }

  std::string get_config(std::string name) {
    msgpack::rpc::future f = c_.call("get_config", name);
    return f.get<std::string>();
  }

  int32_t train(std::string name, std::vector<std::pair<std::string,
       datum> > data) {
    msgpack::rpc::future f = c_.call("train", name, data);
    return f.get<int32_t>();
  }

  std::vector<std::vector<estimate_result> > classify(std::string name,
       std::vector<datum> data) {
    msgpack::rpc::future f = c_.call("classify", name, data);
    return f.get<std::vector<std::vector<estimate_result> > >();
  }

  bool clear(std::string name) {
    msgpack::rpc::future f = c_.call("clear", name);
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

  msgpack::rpc::client& get_client() {
    return c_;
  }

 private:
  msgpack::rpc::client c_;
};

}  // namespace client
}  // namespace classifier
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_CLASSIFIER_CLIENT_HPP_
