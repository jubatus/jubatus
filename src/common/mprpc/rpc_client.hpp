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
#include <glog/logging.h>

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
  rpc_mclient(const std::vector<std::pair<std::string, int> >& hosts,
              int timeout_sec);
  ~rpc_mclient();

  // TODO: make same interface with pficommon's rpc_client
  template <typename Res, typename Argv>
  Res call(const std::string& m, const Argv& a,
           const pfi::lang::function<Res(Res,Res)>& reducer){
    call_async(m, a);
    return join_all(reducer);
  };

  void send_async(const msgpack::sbuffer& buf);

  void call_async(const std::string&);

  template <typename A0>
  void call_async(const std::string&, const A0& a0);
  template <typename A0, typename A1>
  void call_async(const std::string&, const A0& a0, const A1& a1);
  template <typename A0, typename A1, typename A2>
  void call_async(const std::string&, const A0& a0, const A1& a1, const A2& a2);
  template <typename A0, typename A1, typename A2, typename A3>
  void call_async(const std::string&, const A0&, const A1&, const A2&, const A3&);

  template <typename Res>
  Res join_all(const pfi::lang::function<Res(Res,Res)>& reducer);

private:

  template <typename Arr>
  void call_async_(const std::string&, const Arr& a);

  void connect_async_();
  int join_some_(std::vector<msgpack::object>&, int);

  std::vector<std::pair<std::string, uint16_t> > hosts_;
  int timeout_sec_;

  std::vector<pfi::lang::shared_ptr<async_sock> > clients_;
  pfi::system::time::clock_time start_;

  int epfd_;
};

template <typename Arr>
void rpc_mclient::call_async_(const std::string& m, const Arr& argv)
{
  msgpack::sbuffer sbuf;
  msgpack::type::tuple<uint8_t,uint32_t,std::string,Arr> rpc_request(0, 0xDEADBEEF, m, argv);
  msgpack::pack(&sbuf, rpc_request);
  send_async(sbuf);
}

void rpc_mclient::call_async(const std::string& m)
{
  call_async_(m, std::vector<int>());
}

template <typename A0>
void rpc_mclient::call_async(const std::string& m, const A0& a0)
{
  call_async_(m, msgpack::type::tuple<A0>(a0));
}

template <typename A0, typename A1>
void rpc_mclient::call_async(const std::string& m, const A0& a0, const A1& a1)
{
  call_async_(m, msgpack::type::tuple<A0, A1>(a0, a1));
}
template <typename A0, typename A1, typename A2>
void rpc_mclient::call_async(const std::string& m, const A0& a0, const A1& a1, const A2& a2)
{
  call_async_(m, msgpack::type::tuple<A0, A1, A2>(a0, a1, a2));
}
template <typename A0, typename A1, typename A2, typename A3>
void rpc_mclient::call_async(const std::string& m, const A0& a0, const A1& a1, const A2& a2, const A3& a3)
{
  call_async_(m, msgpack::type::tuple<A0, A1, A2, A3>(a0, a1, a2, a3));
}


template <typename Res>
Res rpc_mclient::join_all(const pfi::lang::function<Res(Res,Res)>& reducer)
{

  std::vector<msgpack::object> ret;
  int rest = join_some_(ret, clients_.size());
  if(ret.size()==0){
    throw std::runtime_error("no clients.");
  }
  Res result = ret[0].as<Res>();

  if(ret.size()>1){
    for(size_t i=1;i<ret.size();++i){
      result = reducer(result, ret[i].as<Res>());
    }
  }

  do{

    rest = join_some_(ret, rest);
    for(size_t i=0;i<ret.size();++i){
      result = reducer(result, ret[i].as<Res>());
    }

  }while(rest>0);
  
  return result;
}


}
}
}
