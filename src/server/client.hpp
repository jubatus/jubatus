
#include "classifier_types.hpp"
#include <msgpack/rpc/client.h>


namespace jubatus {

namespace client {

class classifier {
public:
  classifier(const std::string &host, uint64_t port)
    : c_(host, port) {}

    int32_t set_config(std::string name, config_data arg0) {
      return c_.call("set_config", name, arg0).get<int32_t >();
    }

    config_data get_config(std::string name, int32_t arg0) {
      return c_.call("get_config", name, arg0).get<config_data >();
    }

    int32_t train(std::string name, std::vector<std::pair<std::string, datum> > arg0) {
      return c_.call("train", name, arg0).get<int32_t >();
    }

    std::vector<std::vector<estimate_result > > classify(std::string name, std::vector<datum > arg0) {
      return c_.call("classify", name, arg0).get<std::vector<std::vector<estimate_result > > >();
    }

    int32_t save(std::string name, std::string arg0) {
      return c_.call("save", name, arg0).get<int32_t >();
    }

    int32_t load(std::string name, std::string arg0) {
      return c_.call("load", name, arg0).get<int32_t >();
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

