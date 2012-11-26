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

class rpc_response_t {
public:
  rpc_response_t() {}
  rpc_response_t( msgpack::rpc::future f ): future_(f) {}
  
  bool has_error() const { return !future_.error().is_nil(); }
  // uint32_t msgid() const { return 0; /* NOTE: dummy value */ }
  msgpack::object error() const { return future_.error(); }
  template<typename T> const T as() const { return future_.result_as<T>(); }

private:
  msgpack::rpc::future future_;
};

} // mprpc
} // common
} // jubatus
