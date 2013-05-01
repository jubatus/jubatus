// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_COMMON_MPRPC_RPC_RESPONSE_HPP_
#define JUBATUS_SERVER_COMMON_MPRPC_RPC_RESPONSE_HPP_

#include <msgpack.hpp>
#include <jubatus/msgpack/rpc/future.h>

#include <pficommon/lang/shared_ptr.h>

namespace jubatus {
namespace server {
namespace common {
namespace mprpc {

class rpc_response_t {
 public:
  rpc_response_t() {
  }
  explicit rpc_response_t(msgpack::rpc::future f)
      : zone(f.zone().release()) {
    response.a1 = 0;  // NOTE: dummy value
    response.a2 = f.error();
    response.a3 = f.result();
  }

 public:
  msgpack::type::tuple<
      uint8_t,
      uint32_t,
      msgpack::object,
      msgpack::object> response;
  mp::shared_ptr<msgpack::zone> zone;

  bool has_error() const {
    return !response.a2.is_nil();
  }
  uint32_t msgid() const {
    return response.a1;
  }
  msgpack::object& error() {
    return response.a2;
  }
  template<typename T> const T as() const {
    return response.a3.as<T>();
  }
};

}  // namespace mprpc
}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_MPRPC_RPC_RESPONSE_HPP_
