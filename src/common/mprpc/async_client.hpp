// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
#include <map>

#include <msgpack.hpp>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/system/time_util.h>


namespace jubatus { namespace common { namespace mprpc {

class async_sock;

class async_client {
public:
  async_client(const std::string& host, uint16_t port, int timeout_sec);
  ~async_client();
  
  void send_async(const std::string& method, const msgpack::sbuffer& argv);
  void join(msgpack::object& o);
  
  void connect_async();
  
private:
  bool wait();
  
  std::string host_;
  uint16_t port_;
  int timeout_sec_;
  pfi::lang::shared_ptr<async_sock> sock_;
  pfi::system::time::clock_time start_;
};

}
}
}
