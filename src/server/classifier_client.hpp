
#include "classifier_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class classifier : public pfi::network::mprpc::rpc_client {
public:
  classifier(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    int32_t set_config(std::string arg0, config_data arg1) {
      return call<int32_t(std::string, config_data)>("set_config")(arg0, arg1);
    }

    config_data get_config(std::string arg0, int32_t arg1) {
      return call<config_data(std::string, int32_t)>("get_config")(arg0, arg1);
    }

    int32_t train(std::string arg0, std::vector<std::pair<std::string, datum > > arg1) {
      return call<int32_t(std::string, std::vector<std::pair<std::string, datum > >)>("train")(arg0, arg1);
    }

    std::vector<std::vector<estimate_result > > classify(std::string arg0, std::vector<datum > arg1) {
      return call<std::vector<std::vector<estimate_result > >(std::string, std::vector<datum >)>("classify")(arg0, arg1);
    }

    int32_t save(std::string arg0, std::string arg1) {
      return call<int32_t(std::string, std::string)>("save")(arg0, arg1);
    }

    int32_t load(std::string arg0, std::string arg1) {
      return call<int32_t(std::string, std::string)>("load")(arg0, arg1);
    }

    std::map<std::string, std::map<std::string, std::string > > get_status(std::string arg0, int32_t arg1) {
      return call<std::map<std::string, std::map<std::string, std::string > >(std::string, int32_t)>("get_status")(arg0, arg1);
    }

private:
};

} // namespace client

} // namespace jubatus

