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

#pragma once

#include <msgpack.hpp>
#include <pficommon/lang/shared_ptr.h>

namespace jubatus { namespace common { namespace mprpc {

struct rpc_response_t {
  msgpack::type::tuple<uint8_t,uint32_t,msgpack::object,msgpack::object> response;
  pfi::lang::shared_ptr<msgpack::zone> zone;

  bool has_error() const { return !response.a2.is_nil(); }

  uint32_t msgid() const { return response.a1; }
  msgpack::object& error() { return response.a2; }
  template<typename T> const T as() const { return response.a3.as<T>(); }

  rpc_response_t() {}
  rpc_response_t( uint32_t _msgid, const msgpack::object &_error, const msgpack::object &_result,
                  pfi::lang::shared_ptr<msgpack::zone> _zone ) :
    response( /* msgpack::rpc::RESPONSE */ 1, _msgid, _error, _result ),
    zone(_zone) {}
};

} // mprpc
} // common
} // jubatus
