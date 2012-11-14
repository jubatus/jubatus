
// This file is auto-generated from stat.idl
// *** DO NOT EDIT ***

#ifndef STAT_CLIENT_HPP_
#define STAT_CLIENT_HPP_


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

    bool push(std::string name, std::string key, double val) {
      return call<bool(std::string, std::string, double)>("push")(name, key, val);
    }

    double sum(std::string name, std::string key) {
      return call<double(std::string, std::string)>("sum")(name, key);
    }

    double stddev(std::string name, std::string key) {
      return call<double(std::string, std::string)>("stddev")(name, key);
    }

    double max(std::string name, std::string key) {
      return call<double(std::string, std::string)>("max")(name, key);
    }

    double min(std::string name, std::string key) {
      return call<double(std::string, std::string)>("min")(name, key);
    }

    double entropy(std::string name, std::string key) {
      return call<double(std::string, std::string)>("entropy")(name, key);
    }

    double moment(std::string name, std::string key, int32_t n, double c) {
      return call<double(std::string, std::string, int32_t, double)>("moment")(name, key, n, c);
    }

    bool save(std::string name, std::string id) {
      return call<bool(std::string, std::string)>("save")(name, id);
    }

    bool load(std::string name, std::string id) {
      return call<bool(std::string, std::string)>("load")(name, id);
    }

    std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
      return call<std::map<std::string, std::map<std::string, std::string > >(std::string)>("get_status")(name);
    }

private:
};

} // namespace client

} // namespace jubatus



#endif // STAT_CLIENT_HPP_
