
#include "regression_types.hpp"
#include <msgpack/rpc/client.h>


namespace jubatus {

namespace client {

class regression {
public:
  regression(const std::string &host, uint64_t port)
    : c_(host, port) {}

    int32_t set_config(std::string arg0, config_data arg1) {
      return c_.call("set_config", arg0, arg1).get<int32_t >();
    }

    config_data get_config(std::string arg0, int32_t arg1) {
      return c_.call("get_config", arg0, arg1).get<config_data >();
    }

  int32_t train(std::string arg0, std::vector<std::pair<float,datum> > arg1) {
      return c_.call("train", arg0, arg1).get<int32_t >();
    }

    std::vector<float > estimate(std::string arg0, std::vector<datum > arg1) {
      return c_.call("estimate", arg0, arg1).get<std::vector<float > >();
    }

    int32_t save(std::string arg0, std::string arg1) {
      return c_.call("save", arg0, arg1).get<int32_t >();
    }

    int32_t load(std::string arg0, std::string arg1) {
      return c_.call("load", arg0, arg1).get<int32_t >();
    }

    std::string get_diff(int32_t arg0) {
      return c_.call("get_diff", arg0).get<std::string >();
    }

    int32_t put_diff(std::string arg0) {
      return c_.call("put_diff", arg0).get<int32_t >();
    }

private:
  msgpack::rpc::client c_;
};

} // namespace client

} // namespace msgpack

