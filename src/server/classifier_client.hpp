
// This file is auto-generated from classifier.idl
// *** DO NOT EDIT ***

#ifndef CLASSIFIER_CLIENT_HPP_
#define CLASSIFIER_CLIENT_HPP_


#include "classifier_types.hpp"
#include <jubatus/msgpack/rpc/client.h>


namespace jubatus {

namespace client {

class classifier {
public:
  classifier(const std::string &host, uint64_t port, double timeout_sec)
    : c_(host, port) {
    c_.set_timeout( timeout_sec );
  }

  bool set_config(std::string name, config_data c) {
    return c_.call("set_config", name, c).get<bool>();
  }

  config_data get_config(std::string name) {
    return c_.call("get_config", name).get<config_data>();
  }

  int32_t train(std::string name, std::vector<std::pair<std::string, datum > > data) {
    return c_.call("train", name, data).get<int32_t>();
  }

  std::vector<std::vector<estimate_result > > classify(std::string name, std::vector<datum > data) {
    return c_.call("classify", name, data).get<std::vector<std::vector<estimate_result > > >();
  }

  bool save(std::string name, std::string id) {
    return c_.call("save", name, id).get<bool>();
  }

  bool load(std::string name, std::string id) {
    return c_.call("load", name, id).get<bool>();
  }

  std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
    return c_.call("get_status", name).get< std::map<std::string, std::map<std::string, std::string > > >();
  }

private:
  msgpack::rpc::client c_;
};

} // namespace client

} // namespace jubatus



#endif // CLASSIFIER_CLIENT_HPP_
