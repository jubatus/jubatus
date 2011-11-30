
#include "classifier_types.hpp"
#include <msgpack/rpc/client.h>


namespace msgpack {

namespace client {

class classifier {
public:
  classifier(const std::string &host, uint64_t port)
    : c_(host, port) {}

    int32_t set_config(config_data arg0) {
      return c_.call("set_config", arg0).get<int32_t >();
    }

    config_data get_config(int32_t arg0) {
      return c_.call("get_config", arg0).get<config_data >();
    }

    int32_t save(std::string arg0) {
      return c_.call("save", arg0).get<int32_t >();
    }

    int32_t load(std::string arg0) {
      return c_.call("load", arg0).get<int32_t >();
    }

  int32_t train(std::vector<std::pair<std::string, datum> > arg0) {
      return c_.call("train", arg0).get<int32_t >();
    }

    std::vector<std::vector<estimate_result > > classify(std::vector<datum > arg0) {
      return c_.call("classify", arg0).get<std::vector<std::vector<estimate_result > > >();
    }

private:
  msgpack::rpc::client c_;
};

} // namespace client

} // namespace msgpack

