// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <pficommon/data/unordered_map.h>
#include <pficommon/lang/noncopyable.h>
#include <pficommon/network/mprpc/exception.h>

#include <event.h>

namespace jubatus { namespace common { namespace mprpc {

class rpc_mclient;
struct async_context {
  rpc_mclient* c;
  event_base* evbase;
  const msgpack::sbuffer* buf;
  size_t rest;
  std::vector<msgpack::object> ret;
};

struct socket_context {
  event ev_read;
  event ev_write;
  pfi::lang::shared_ptr<async_sock> socket;

  socket_context(pfi::lang::shared_ptr<async_sock> s)
    : socket(s)
  {
    memset(&ev_read, 0, sizeof(event));
    memset(&ev_write, 0, sizeof(event));
  }

  socket_context()
  {
    memset(&ev_read, 0, sizeof(event));
    memset(&ev_write, 0, sizeof(event));
  }
};

class rpc_mclient : pfi::lang::noncopyable
{
public:
  rpc_mclient(const std::vector<std::pair<std::string, uint16_t> >& hosts,
              int timeout_sec);
  rpc_mclient(const std::vector<std::pair<std::string, int> >& hosts,
              int timeout_sec);
  ~rpc_mclient();

  template <typename Res, typename Argv>
  Res call(const std::string& m, const Argv& a,
           const pfi::lang::function<Res(Res,Res)>& reducer){
    call_async(m, a);
    return join_all(reducer);
  }

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

  int readable_callback(int, int, async_context*);
  int writable_callback(int, int, async_context*);


private:
  void create_fd_event_();
  void register_fd_readable_();
  void register_fd_writable_();

  template <typename Arr>
  void call_async_(const std::string&, const Arr& a);

  void connect_async_();
  void join_some_();

  std::vector<std::pair<std::string, uint16_t> > hosts_;
  int timeout_sec_;

  typedef pfi::data::unordered_map<int, socket_context> socket_info_map_t;
  socket_info_map_t clients_;
  pfi::system::time::clock_time start_;

  // WARN: Don't move context_ address which is referenced
  // by readable_callback and writable_callback.
  async_context context_;

  event_base* evbase_;
};

template <typename Arr>
void rpc_mclient::call_async_(const std::string& m, const Arr& argv)
{
  msgpack::sbuffer sbuf;
  msgpack::type::tuple<uint8_t,uint32_t,std::string,Arr> rpc_request(0, 0xDEADBEEF, m, argv);
  msgpack::pack(&sbuf, rpc_request);
  send_async(sbuf);
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
  if (clients_.empty())
    throw pfi::network::mprpc::rpc_error("no clients.");

  context_ = async_context();
  context_.c = this;
  context_.evbase = evbase_;
  context_.rest = clients_.size();
  context_.buf = NULL;

  register_fd_readable_();

  join_some_();
  if (context_.ret.empty())
    throw pfi::network::mprpc::rpc_error("no results.");

  Res result = context_.ret[0].as<Res>();
  for(size_t i=1;i<context_.ret.size();++i){
    result = reducer(result, context_.ret[i].as<Res>());
  }

  return result;
}

}
}
}
