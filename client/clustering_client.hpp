// This file is auto-generated from clustering.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLUSTERING_CLIENT_HPP_
#define JUBATUS_CLUSTERING_CLIENT_HPP_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <jubatus/msgpack/rpc/client.h>
#include "clustering_types.hpp"

namespace jubatus {
namespace clustering {
namespace client {

class clustering {
 public:
  clustering(const std::string& host, uint64_t port, double timeout_sec)
      : c_(host, port) {
    c_.set_timeout(timeout_sec);
  }
  
  std::string get_config(std::string name) {
    msgpack::rpc::future f = c_.call("get_config", name);
    return f.get<std::string>();
  }
  
  bool push(std::string name, std::vector<datum> points) {
    msgpack::rpc::future f = c_.call("push", name, points);
    return f.get<bool>();
  }
  
  uint32_t get_revision(std::string name) {
    msgpack::rpc::future f = c_.call("get_revision", name);
    return f.get<uint32_t>();
  }
  
  std::vector<std::vector<std::pair<double, datum> > > get_core_members(
      std::string name) {
    msgpack::rpc::future f = c_.call("get_core_members", name);
    return f.get<std::vector<std::vector<std::pair<double, datum> > > >();
  }
  
  std::vector<datum> get_k_center(std::string name) {
    msgpack::rpc::future f = c_.call("get_k_center", name);
    return f.get<std::vector<datum> >();
  }
  
  datum get_nearest_center(std::string name, datum point) {
    msgpack::rpc::future f = c_.call("get_nearest_center", name, point);
    return f.get<datum>();
  }
  
  std::vector<std::pair<double, datum> > get_nearest_members(std::string name,
       datum point) {
    msgpack::rpc::future f = c_.call("get_nearest_members", name, point);
    return f.get<std::vector<std::pair<double, datum> > >();
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
}  // namespace clustering
}  // namespace jubatus

#endif  // JUBATUS_CLUSTERING_CLIENT_HPP_
