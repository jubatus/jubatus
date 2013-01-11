
// This file is auto-generated from recommender.idl
// *** DO NOT EDIT ***

#ifndef RECOMMENDER_CLIENT_HPP_
#define RECOMMENDER_CLIENT_HPP_


#include "recommender_types.hpp"
#include <jubatus/msgpack/rpc/client.h>


namespace jubatus {

namespace client {

class recommender {
public:
  recommender(const std::string &host, uint64_t port, double timeout_sec)
    : c_(host, port) {
    c_.set_timeout( timeout_sec );
  }

  std::string get_config(std::string name) {
    return c_.call("get_config", name).get<std::string>();
  }

  bool clear_row(std::string name, std::string id) {
    return c_.call("clear_row", name, id).get<bool>();
  }

  bool update_row(std::string name, std::string id, datum d) {
    return c_.call("update_row", name, id, d).get<bool>();
  }

  bool clear(std::string name) {
    return c_.call("clear", name).get<bool>();
  }

  datum complete_row_from_id(std::string name, std::string id) {
    return c_.call("complete_row_from_id", name, id).get<datum>();
  }

  datum complete_row_from_datum(std::string name, datum d) {
    return c_.call("complete_row_from_datum", name, d).get<datum>();
  }

  similar_result similar_row_from_id(std::string name, std::string id, uint32_t size) {
    return c_.call("similar_row_from_id", name, id, size).get<similar_result>();
  }

  similar_result similar_row_from_datum(std::string name, datum data, uint32_t size) {
    return c_.call("similar_row_from_datum", name, data, size).get<similar_result>();
  }

  datum decode_row(std::string name, std::string id) {
    return c_.call("decode_row", name, id).get<datum>();
  }

  std::vector<std::string > get_all_rows(std::string name) {
    return c_.call("get_all_rows", name).get<std::vector<std::string > >();
  }

  float calc_similarity(std::string name, datum lhs, datum rhs) {
    return c_.call("calc_similarity", name, lhs, rhs).get<float>();
  }

  float calc_l2norm(std::string name, datum d) {
    return c_.call("calc_l2norm", name, d).get<float>();
  }

  bool save(std::string name, std::string id) {
    return c_.call("save", name, id).get<bool>();
  }

  bool load(std::string name, std::string id) {
    return c_.call("load", name, id).get<bool>();
  }

  std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
    return c_.call("get_status", name).get<std::map<std::string, std::map<std::string, std::string > > >();
  }

private:
  msgpack::rpc::client c_;
};

} // namespace client

} // namespace jubatus



#endif // RECOMMENDER_CLIENT_HPP_
