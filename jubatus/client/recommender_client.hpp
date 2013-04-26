// This file is auto-generated from recommender.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_RECOMMENDER_CLIENT_HPP_
#define JUBATUS_CLIENT_RECOMMENDER_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <jubatus/msgpack/rpc/client.h>
#include "recommender_types.hpp"

namespace jubatus {
namespace recommender {
namespace client {

class recommender {
 public:
  recommender(const std::string& host, uint64_t port, double timeout_sec)
      : c_(host, port) {
    c_.set_timeout(timeout_sec);
  }

  std::string get_config(std::string name) {
    msgpack::rpc::future f = c_.call("get_config", name);
    return f.get<std::string>();
  }

  bool clear_row(std::string name, std::string id) {
    msgpack::rpc::future f = c_.call("clear_row", name, id);
    return f.get<bool>();
  }

  bool update_row(std::string name, std::string id, datum row) {
    msgpack::rpc::future f = c_.call("update_row", name, id, row);
    return f.get<bool>();
  }

  bool clear(std::string name) {
    msgpack::rpc::future f = c_.call("clear", name);
    return f.get<bool>();
  }

  datum complete_row_from_id(std::string name, std::string id) {
    msgpack::rpc::future f = c_.call("complete_row_from_id", name, id);
    return f.get<datum>();
  }

  datum complete_row_from_datum(std::string name, datum row) {
    msgpack::rpc::future f = c_.call("complete_row_from_datum", name, row);
    return f.get<datum>();
  }

  similar_result similar_row_from_id(std::string name, std::string id,
       uint32_t size) {
    msgpack::rpc::future f = c_.call("similar_row_from_id", name, id, size);
    return f.get<similar_result>();
  }

  similar_result similar_row_from_datum(std::string name, datum row,
       uint32_t size) {
    msgpack::rpc::future f = c_.call("similar_row_from_datum", name, row, size);
    return f.get<similar_result>();
  }

  datum decode_row(std::string name, std::string id) {
    msgpack::rpc::future f = c_.call("decode_row", name, id);
    return f.get<datum>();
  }

  std::vector<std::string> get_all_rows(std::string name) {
    msgpack::rpc::future f = c_.call("get_all_rows", name);
    return f.get<std::vector<std::string> >();
  }

  float calc_similarity(std::string name, datum lhs, datum rhs) {
    msgpack::rpc::future f = c_.call("calc_similarity", name, lhs, rhs);
    return f.get<float>();
  }

  float calc_l2norm(std::string name, datum row) {
    msgpack::rpc::future f = c_.call("calc_l2norm", name, row);
    return f.get<float>();
  }

  bool save(std::string name, std::string id) {
    msgpack::rpc::future f = c_.call("save", name, id);
    return f.get<bool>();
  }

  bool load(std::string name, std::string id) {
    msgpack::rpc::future f = c_.call("load", name, id);
    return f.get<bool>();
  }

  std::map<std::string, std::map<std::string, std::string> > get_status(
      std::string name) {
    msgpack::rpc::future f = c_.call("get_status", name);
    return f.get<std::map<std::string, std::map<std::string, std::string> > >();
  }

  msgpack::rpc::client& get_client() {
    return c_;
  }

 private:
  msgpack::rpc::client c_;
};

}  // namespace client
}  // namespace recommender
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_RECOMMENDER_CLIENT_HPP_
