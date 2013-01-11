
// This file is auto-generated from regression.idl
// *** DO NOT EDIT ***

#ifndef REGRESSION_CLIENT_HPP_
#define REGRESSION_CLIENT_HPP_


#include "regression_types.hpp"
#include <jubatus/msgpack/rpc/client.h>

namespace jubatus {

namespace client {

class regression {
public:
  regression(const std::string &host, uint64_t port, double timeout_sec)
    : c_(host, port) {
    c_.set_timeout( timeout_sec );
  }

  std::string get_config(std::string name) {
    return c_.call("get_config", name).get<std::string>();
  }

  int32_t train(std::string name, std::vector<std::pair<float, datum > > train_data) {
    return c_.call("train", name, train_data).get<int32_t>();
  }

  std::vector<float > estimate(std::string name, std::vector<datum > estimate_data) {
    return c_.call("estimate", name, estimate_data).get<std::vector<float> >();
  }

  bool save(std::string name, std::string arg1) {
    return c_.call("save", name, arg1).get<bool>();
  }

  bool load(std::string name, std::string arg1) {
    return c_.call("load", name, arg1).get<bool>();
  }

  std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
    return c_.call("get_status", name).get<std::map<std::string, std::map<std::string, std::string> > >();
  }

private:
  msgpack::rpc::client c_;
};

} // namespace client

} // namespace jubatus



#endif // REGRESSION_CLIENT_HPP_
