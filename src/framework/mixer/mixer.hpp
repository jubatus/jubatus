// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/network/mprpc.h>
#include "../server_base.hpp"
#include "../../common/mprpc/rpc_server2.hpp"

namespace jubatus {
namespace framework {

class mixable0;
class mixable_holder;

namespace mixer {

class mixer {
public:
#if 0
  typedef pfi::network::mprpc::rpc_server rpc_server_t;
#endif
  typedef jubatus::common::mprpc::rpc_server2 rpc_server_t;

public:
  virtual ~mixer() {}

  virtual void register_api(rpc_server_t& server) = 0;
  virtual void set_mixable_holder(pfi::lang::shared_ptr<mixable_holder>) = 0;

  virtual void start() = 0;
  virtual void stop() = 0;

  virtual void updated() = 0;

  virtual void get_status(server_base::status_t& status) const = 0;
};

}
}
}
