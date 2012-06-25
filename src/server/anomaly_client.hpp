
#include "anomaly_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class anomaly : public pfi::network::mprpc::rpc_client {
public:
  anomaly(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    bool set_config(std::string name, config_data c) {
      return call<bool(std::string, config_data)>("set_config")(name, c);
    }

    config_data get_config(std::string name) {
      return call<config_data(std::string)>("get_config")(name);
    }

    bool clear_row(std::string name, std::string id) {
      return call<bool(std::string, std::string)>("clear_row")(name, id);
    }

    std::pair<std::string, float > add(std::string name, datum d) {
      return call<std::pair<std::string, float >(std::string, datum)>("add")(name, d);
    }

    float update(std::string name, std::string id, datum d) {
      return call<float(std::string, std::string, datum)>("update")(name, id, d);
    }

    bool clear(std::string name) {
      return call<bool(std::string)>("clear")(name);
    }

    float calc_score(std::string name, datum d) {
      return call<float(std::string, datum)>("calc_score")(name, d);
    }

    std::vector<std::string > get_all_rows(std::string name) {
      return call<std::vector<std::string >(std::string)>("get_all_rows")(name);
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

