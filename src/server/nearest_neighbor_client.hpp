// This file is auto-generated from nearest_neighbor.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_NEAREST_NEIGHBOR_CLIENT_HPP_
#define JUBATUS_SERVER_NEAREST_NEIGHBOR_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <jubatus/msgpack/rpc/client.h>
#include "nearest_neighbor_types.hpp"

namespace jubatus {
namespace client {

class nearest_neighbor {
 public:
  nearest_neighbor(const std::string& host, uint64_t port, double timeout_sec)
      : c_(host, port) {
    c_.set_timeout(timeout_sec);
  }
  
  bool init_table(std::string name) {
    msgpack::rpc::future f = c_.call("init_table", name);
    return f.get<bool>();
  }
  
  bool clear(std::string name) {
    msgpack::rpc::future f = c_.call("clear", name);
    return f.get<bool>();
  }
  
  bool set_row(std::string name, std::string id, datum d) {
    msgpack::rpc::future f = c_.call("set_row", name, id, d);
    return f.get<bool>();
  }
  
  neighbor_result neighbor_row_from_id(std::string name, std::string id,
       uint32_t size) {
    msgpack::rpc::future f = c_.call("neighbor_row_from_id", name, id, size);
    return f.get<neighbor_result>();
  }
  
  neighbor_result neighbor_row_from_data(std::string name, datum query,
       uint32_t size) {
    msgpack::rpc::future f = c_.call("neighbor_row_from_data", name, query,
         size);
    return f.get<neighbor_result>();
  }
  
  neighbor_result similar_row_from_id(std::string name, std::string id,
       int32_t ret_num) {
    msgpack::rpc::future f = c_.call("similar_row_from_id", name, id, ret_num);
    return f.get<neighbor_result>();
  }
  
  neighbor_result similar_row_from_data(std::string name, datum query,
       int32_t ret_num) {
    msgpack::rpc::future f = c_.call("similar_row_from_data", name, query,
         ret_num);
    return f.get<neighbor_result>();
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
}  // namespace jubatus

#endif  // JUBATUS_SERVER_NEAREST_NEIGHBOR_CLIENT_HPP_
