
// This file is auto-generated from recommender.idl
// *** DO NOT EDIT ***

#ifndef RECOMMENDER_CLIENT_HPP_
#define RECOMMENDER_CLIENT_HPP_


#include "recommender_types.hpp"
#include <msgpack/rpc/client.h>


namespace jubatus {

namespace client {

class recommender {
public:
  recommender(const std::string &host, uint64_t port, double timeout_sec)
    : c_(host, port) {
    c_.set_timeout( timeout_sec );
  }

  bool set_config(std::string name, config_data c) {
    return c_.call("set_config", name, c).get<bool>();
  }

  config_data get_config(std::string name) {
    return c_.call("get_config", name).get<config_data>();
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

  datum complete_row_from_data(std::string name, datum d) {
    return c_.call("complete_row_from_data", name, d).get<datum>();
  }

  similar_result similar_row_from_id(std::string name, std::string id, uint32_t size) {
    return c_.call("similar_row_from_id", name, id, size).get<similar_result>();
  }

  similar_result similar_row_from_data(std::string name, datum data, uint32_t size) {
    return c_.call("similar_row_from_data", name, data, size).get<similar_result>();
  }

  datum decode_row(std::string name, std::string id) {
    return c_.call("decode_row", name, id).get<datum>();
  }

  std::vector<std::string > get_all_rows(std::string name) {
    return c_.call("get_all_rows", name).get<std::vector<std::string > >();
  }

  float similarity(std::string name, datum lhs, datum rhs) {
    return c_.call("similarity", name, lhs, rhs).get<float>();
  }

  float l2norm(std::string name, datum d) {
    return c_.call("l2norm", name, d).get<float>();
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
