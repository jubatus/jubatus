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

#include <string>
#include <vector>

#include <pficommon/lang/shared_ptr.h>
#include "rpc_response.hpp"
#include "rpc_error.hpp"

namespace jubatus { namespace common { namespace mprpc {

template <class Res>
struct rpc_result {
  pfi::lang::shared_ptr<Res> value;
  std::vector<rpc_error> error;

  Res& operator*() const { return *value; }
  bool has_error() const { return !error.empty(); }
};

struct rpc_result_object {
  std::vector<rpc_response_t> response;
  std::vector<rpc_error> error;

  bool has_error() const { return !error.empty(); }
};

} // mprpc
} // common
} // jubatus
