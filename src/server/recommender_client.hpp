
// This file is auto-generated from recommender.idl
// *** DO NOT EDIT ***

#ifndef RECOMMENDER_CLIENT_HPP_
#define RECOMMENDER_CLIENT_HPP_


#include "recommender_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class recommender : public pfi::network::mprpc::rpc_client {
public:
  recommender(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    std::string get_config(std::string name) {
      return call<std::string(std::string)>("get_config")(name);
    }

    bool clear_row(std::string name, std::string id) {
      return call<bool(std::string, std::string)>("clear_row")(name, id);
    }

    bool update_row(std::string name, std::string id, datum d) {
      return call<bool(std::string, std::string, datum)>("update_row")(name, id, d);
    }

    bool clear(std::string name) {
      return call<bool(std::string)>("clear")(name);
    }

    datum complete_row_from_id(std::string name, std::string id) {
      return call<datum(std::string, std::string)>("complete_row_from_id")(name, id);
    }

    datum complete_row_from_data(std::string name, datum d) {
      return call<datum(std::string, datum)>("complete_row_from_data")(name, d);
    }

    similar_result similar_row_from_id(std::string name, std::string id, uint32_t size) {
      return call<similar_result(std::string, std::string, uint32_t)>("similar_row_from_id")(name, id, size);
    }

    similar_result similar_row_from_data(std::string name, datum data, uint32_t size) {
      return call<similar_result(std::string, datum, uint32_t)>("similar_row_from_data")(name, data, size);
    }

    datum decode_row(std::string name, std::string id) {
      return call<datum(std::string, std::string)>("decode_row")(name, id);
    }

    std::vector<std::string > get_all_rows(std::string name) {
      return call<std::vector<std::string >(std::string)>("get_all_rows")(name);
    }

    float similarity(std::string name, datum lhs, datum rhs) {
      return call<float(std::string, datum, datum)>("similarity")(name, lhs, rhs);
    }

    float l2norm(std::string name, datum d) {
      return call<float(std::string, datum)>("l2norm")(name, d);
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



#endif // RECOMMENDER_CLIENT_HPP_
