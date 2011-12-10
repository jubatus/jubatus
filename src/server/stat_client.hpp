
#include "stat_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class stat : public pfi::network::mprpc::rpc_client {
public:
  stat(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    int32_t set_config(std::string arg0, config_data arg1) {
      return call<int32_t(std::string, config_data)>("set_config")(arg0, arg1);
    }

    int32_t push(std::string arg0, std::string arg1, double arg2) {
      return call<int32_t(std::string, std::string, double)>("push")(arg0, arg1, arg2);
    }

    double sum(std::string arg0, std::string arg1, int32_t arg2) {
      return call<double(std::string, std::string, int32_t)>("sum")(arg0, arg1, arg2);
    }

    double stddev(std::string arg0, std::string arg1, int32_t arg2) {
      return call<double(std::string, std::string, int32_t)>("stddev")(arg0, arg1, arg2);
    }

    double max(std::string arg0, std::string arg1, int32_t arg2) {
      return call<double(std::string, std::string, int32_t)>("max")(arg0, arg1, arg2);
    }

    double min(std::string arg0, std::string arg1, int32_t arg2) {
      return call<double(std::string, std::string, int32_t)>("min")(arg0, arg1, arg2);
    }

    double entropy(std::string arg0, std::string arg1, int32_t arg2) {
      return call<double(std::string, std::string, int32_t)>("entropy")(arg0, arg1, arg2);
    }

    double moment(std::string arg0, std::string arg1, std::pair<int32_t, double > arg2) {
      return call<double(std::string, std::string, std::pair<int32_t, double >)>("moment")(arg0, arg1, arg2);
    }

    int32_t save(std::string arg0, std::string arg1) {
      return call<int32_t(std::string, std::string)>("save")(arg0, arg1);
    }

    int32_t load(std::string arg0, std::string arg1) {
      return call<int32_t(std::string, std::string)>("load")(arg0, arg1);
    }

private:
};

} // namespace client

} // namespace jubatus

