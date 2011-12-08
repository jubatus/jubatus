
#include "recommender_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class recommender : public pfi::network::mprpc::rpc_client {
public:
  recommender(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    int32_t set_config(std::string arg0, config_data arg1) {
      return call<int32_t(std::string, config_data)>("set_config")(arg0, arg1);
    }

    config_data get_config(std::string arg0, int32_t arg1) {
      return call<config_data(std::string, int32_t)>("get_config")(arg0, arg1);
    }

    int32_t clear_row(std::string arg0, std::string arg1, int32_t arg2) {
      return call<int32_t(std::string, std::string, int32_t)>("clear_row")(arg0, arg1, arg2);
    }

    int32_t update_row(std::string arg0, std::string arg1, datum arg2) {
      return call<int32_t(std::string, std::string, datum)>("update_row")(arg0, arg1, arg2);
    }

    int32_t build(std::string arg0, int32_t arg1) {
      return call<int32_t(std::string, int32_t)>("build")(arg0, arg1);
    }

    int32_t clear(std::string arg0, int32_t arg1) {
      return call<int32_t(std::string, int32_t)>("clear")(arg0, arg1);
    }

    datum complete_row_from_id(std::string arg0, std::string arg1, int32_t arg2) {
      return call<datum(std::string, std::string, int32_t)>("complete_row_from_id")(arg0, arg1, arg2);
    }

    datum complete_row_from_data(std::string arg0, datum arg1) {
      return call<datum(std::string, datum)>("complete_row_from_data")(arg0, arg1);
    }

    similar_result similar_row_from_id(std::string arg0, std::string arg1, size_t arg2) {
      return call<similar_result(std::string, std::string, size_t)>("similar_row_from_id")(arg0, arg1, arg2);
    }

    similar_result similar_row_from_data(std::string arg0, std::pair<datum, size_t > arg1) {
      return call<similar_result(std::string, std::pair<datum, size_t >)>("similar_row_from_data")(arg0, arg1);
    }

    datum decode_row(std::string arg0, std::string arg1, int32_t arg2) {
      return call<datum(std::string, std::string, int32_t)>("decode_row")(arg0, arg1, arg2);
    }

    std::vector<std::pair<std::string, datum > > get_all_rows(std::string arg0, int32_t arg1) {
      return call<std::vector<std::pair<std::string, datum > >(std::string, int32_t)>("get_all_rows")(arg0, arg1);
    }

    int32_t save(std::string arg0, std::string arg1) {
      return call<int32_t(std::string, std::string)>("save")(arg0, arg1);
    }

    int32_t load(std::string arg0, std::string arg1) {
      return call<int32_t(std::string, std::string)>("load")(arg0, arg1);
    }

    std::string get_diff(int32_t arg0) {
      return call<std::string(int32_t)>("get_diff")(arg0);
    }

    int32_t put_diff(std::string arg0) {
      return call<int32_t(std::string)>("put_diff")(arg0);
    }

private:
};

} // namespace client

} // namespace jubatus

