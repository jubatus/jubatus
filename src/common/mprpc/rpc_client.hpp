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
#include <pficommon/lang/function.h>
#include <pficommon/system/time_util.h>
#include "async_client.hpp"

namespace jubatus { namespace common { namespace mprpc {

class rpc_client {
public:
  rpc_client(const std::string& host, uint16_t port, int timeout_sec):
    cli_(host, port, timeout_sec)
  {};
  ~rpc_client();
  
  template <typename Argv, typename Res> // TODO: make same interface with pficommon's rpc_client
  Res call(const std::string&, const Argv& a);
  
private:
  async_client cli_;
};

template <typename Argv, typename Res>
Res rpc_client::call(const std::string& method, const Argv& a){

  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, a);
  cli_.send_async(method, a);

  msgpack::object ret;
  cli_.join(ret);
  Res result;
  ret.convert(&result);
  return result;

}

class rpc_mclient {
public:
  rpc_mclient(const std::vector<std::pair<std::string, uint16_t> >& hosts,
              int timeout_sec);
  ~rpc_mclient();

  template <typename Argv, typename Res> // TODO: make same interface with pficommon's rpc_client
  Res call(const std::string&, const Argv& a){ Res r; return r; };

  void send_async(const std::string&, const msgpack::sbuffer& buf);
  template <typename Res>
  Res join_all(const pfi::lang::function<Res(Res,Res)>& reducer);
      
private:
  std::vector<std::pair<std::string, uint16_t> > hosts_;
  std::vector<async_client> clients_;
  int timeout_sec_;
};

template <typename Res>
Res rpc_mclient::join_all(const pfi::lang::function<Res(Res,Res)>& reducer)
{
  Res result;
  {
    msgpack::object ret;
    clients_[0].join(ret);
    ret.convert(result);
  }

  for(size_t i=1; i<clients_.size(); ++i){
    msgpack::object ret;
    clients_[i].join(ret);
    Res tmp;
    ret.convert(tmp);
    result = reducer(result, ret);
  }
  return result;
}


}
}
}
