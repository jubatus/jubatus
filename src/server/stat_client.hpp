
#include "stat_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class stat : public pfi::network::mprpc::rpc_client {
public:
  stat(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    bool set_config(std::string name, config_data c) {
      return call<bool(std::string, config_data)>("set_config")(name, c);
    }

    config_data get_config(std::string name) {
      return call<config_data(std::string)>("get_config")(name);
    }

    bool push(std::string arg0, std::string arg1, double arg2) {
      return call<bool(std::string, std::string, double)>("push")(arg0, arg1, arg2);
    }

    double sum(std::string arg0, std::string arg1) {
      return call<double(std::string, std::string)>("sum")(arg0, arg1);
    }

    double stddev(std::string arg0, std::string arg1) {
      return call<double(std::string, std::string)>("stddev")(arg0, arg1);
    }

    double max(std::string arg0, std::string arg1) {
      return call<double(std::string, std::string)>("max")(arg0, arg1);
    }

    double min(std::string arg0, std::string arg1) {
      return call<double(std::string, std::string)>("min")(arg0, arg1);
    }

    double entropy(std::string arg0, std::string arg1) {
      return call<double(std::string, std::string)>("entropy")(arg0, arg1);
    }

    double moment(std::string arg0, std::string arg1, int32_t n, double c) {
      return call<double(std::string, std::string, int32_t, double)>("moment")(arg0, arg1, n, c);
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

