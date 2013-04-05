
#include "nearest_neighbor_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class nearest_neighbor : public pfi::network::mprpc::rpc_client {
public:
  nearest_neighbor(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    bool set_config(std::string name, config_data config) {
      return call<bool(std::string, config_data)>("set_config")(name, config);
    }

    config_data get_config(std::string name) {
      return call<config_data(std::string)>("get_config")(name);
    }

    bool init_table(std::string name) {
      return call<bool(std::string)>("init_table")(name);
    }

    bool clear(std::string name) {
      return call<bool(std::string)>("clear")(name);
    }

    bool set_row(std::string name, std::string id, datum d) {
      return call<bool(std::string, std::string, datum)>("set_row")(name, id, d);
    }

    neighbor_result neighbor_row_from_id(std::string name, std::string id, uint32_t size) {
      return call<neighbor_result(std::string, std::string, uint32_t)>("neighbor_row_from_id")(name, id, size);
    }

    neighbor_result neighbor_row_from_data(std::string name, datum query, uint32_t size) {
      return call<neighbor_result(std::string, datum, uint32_t)>("neighbor_row_from_data")(name, query, size);
    }

    neighbor_result similar_row_from_id(std::string name, std::string id, int32_t ret_num) {
      return call<neighbor_result(std::string, std::string, int32_t)>("similar_row_from_id")(name, id, ret_num);
    }

    neighbor_result similar_row_from_data(std::string name, datum query, int32_t ret_num) {
      return call<neighbor_result(std::string, datum, int32_t)>("similar_row_from_data")(name, query, ret_num);
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

