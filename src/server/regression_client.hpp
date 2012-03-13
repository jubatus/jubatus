
#include "regression_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class regression : public pfi::network::mprpc::rpc_client {
public:
  regression(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    bool set_config(std::string name, config_data c) {
      return call<bool(std::string, config_data)>("set_config")(name, c);
    }

    config_data get_config(std::string name) {
      return call<config_data(std::string)>("get_config")(name);
    }

    int32_t train(std::string arg0, std::vector<std::pair<float, datum > > arg1) {
      return call<int32_t(std::string, std::vector<std::pair<float, datum > >)>("train")(arg0, arg1);
    }

    std::vector<float > estimate(std::string arg0, std::vector<datum > arg1) {
      return call<std::vector<float >(std::string, std::vector<datum >)>("estimate")(arg0, arg1);
    }

    bool save(std::string name, std::string arg1) {
      return call<bool(std::string, std::string)>("save")(name, arg1);
    }

    bool load(std::string name, std::string arg1) {
      return call<bool(std::string, std::string)>("load")(name, arg1);
    }

    std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
      return call<std::map<std::string, std::map<std::string, std::string > >(std::string)>("get_status")(name);
    }

private:
};

} // namespace client

} // namespace jubatus

