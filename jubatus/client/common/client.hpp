// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_CLIENT_COMMON_CLIENT_HPP_
#define JUBATUS_CLIENT_COMMON_CLIENT_HPP_

#include <stdint.h>
#include <map>
#include <string>
#include <jubatus/msgpack/rpc/client.h>

namespace jubatus {
namespace client {
namespace common {

class client {
 public:
  client(const std::string& host,
         uint64_t port,
         const std::string& name,
         unsigned int timeout_sec)
      : c_(host, port), name_(name) {
    c_.set_timeout(timeout_sec);
  }

  msgpack::rpc::client& get_client() {
    return c_;
  }

  std::string get_config() {
    msgpack::rpc::future f = c_.call("get_config", name_);
    return f.get<std::string>();
  }

  bool save(const std::string& id) {
    msgpack::rpc::future f = c_.call("save", name_, id);
    return f.get<bool>();
  }

  bool load(const std::string& id) {
    msgpack::rpc::future f = c_.call("load", name_, id);
    return f.get<bool>();
  }

  std::map<std::string, std::map<std::string, std::string> > get_status() {
    msgpack::rpc::future f = c_.call("get_status", name_);
    return f.get<std::map<std::string, std::map<std::string, std::string> > >();
  }

 protected:
  msgpack::rpc::client c_;
  std::string name_;
};

}  // namespace common
}  // namespace client
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_COMMON_CLIENT_HPP_
