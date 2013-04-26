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

#ifndef JUBATUS_SERVER_COMMON_MPRPC_RPC_SERVER_HPP_
#define JUBATUS_SERVER_COMMON_MPRPC_RPC_SERVER_HPP_

#include <map>
#include <string>
#include <jubatus/msgpack/rpc/server.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/function.h>

namespace jubatus {
namespace server {
namespace common {
namespace mprpc {

// invoker_base, invoker*
//   method entry for rpc_server ( internal use only ).
//   invoker_base is the base class. invokerN is entry for arity-N method
class invoker_base {
 public:
  typedef msgpack::rpc::request request_type;

  virtual ~invoker_base() {
  }
  virtual void invoke(request_type& req) = 0;
};

// async var-arg method type
template<typename Tuple>
struct async_vmethod {
  typedef pfi::lang::function<void(invoker_base::request_type, Tuple)> type;
};

// rpc_server
//   Msgpack-RPC based server with 'hashed' dispatcher.
//   rpc_server can add RPC method on-the-fly.
//
class rpc_server : public msgpack::rpc::dispatcher {
 public:
  rpc_server(msgpack::rpc::loop lo = msgpack::rpc::loop())
      : instance_(lo) {
    instance_.serve(this);
  }
  rpc_server(
      double server_timeout,
      msgpack::rpc::loop lo = msgpack::rpc::loop())
      : instance_(lo) {
    instance_.set_server_timeout(server_timeout);
    instance_.serve(this);
  }
  ~rpc_server() {
  }

  virtual void dispatch(msgpack::rpc::request req);

  // synchronous method registration
  template<typename T> void add(
      const std::string& name,
      const pfi::lang::function<T>& f);

  // *asynchronous* *var-arg* method registration
  //   where var-arg method means a method receive its arguments
  //   by one packed tuple
  template<typename Tuple>
  void add_async_vmethod(
      const std::string& name,
      const typename async_vmethod<Tuple>::type& f);

  void listen(uint16_t port);
  void listen(uint16_t port, const std::string& bind_address);
  void start(int nthreads, bool no_hang = false);
  void join();
  void stop();
  void close();

  msgpack::rpc::server instance_;

 private:
  typedef std::map<std::string, pfi::lang::shared_ptr<invoker_base> > func_map;

  void add_inner(
      const std::string& name,
      pfi::lang::shared_ptr<invoker_base> invoker);

  func_map funcs_;
};

//
// synchronous method invokers ( arity 0-4 )
//
template<typename R>
class invoker0 : public invoker_base {
 public:
  typedef pfi::lang::function<R()> func_type;

  explicit invoker0(const func_type& f)
      : f_(f) {
  }
  virtual void invoke(msgpack::rpc::request& req) {
    R retval = f_();
    req.result<R>(retval);
  }

 private:
  func_type f_;
};

template<typename R>
pfi::lang::shared_ptr<invoker_base> make_invoker(
    const pfi::lang::function<R()>& f) {
  return pfi::lang::shared_ptr<invoker_base>(new invoker0<R>(f));
}

template<typename R, typename A1>
class invoker1 : public invoker_base {
 public:
  typedef pfi::lang::function<R(A1)> func_type;

  explicit invoker1(const func_type& f)
      : f_(f) {
  }
  virtual void invoke(msgpack::rpc::request& req) {
    msgpack::type::tuple<A1> params;
    req.params().convert(&params);
    R retval = f_(params.template get<0>());
    req.result<R> (retval);
  }

 private:
  func_type f_;
};

template<typename R, typename A1>
pfi::lang::shared_ptr<invoker_base> make_invoker(
    const pfi::lang::function<R(A1)>& f) {
  return pfi::lang::shared_ptr<invoker_base>(new invoker1<R, A1>(f));
}

template<typename R, typename A1, typename A2>
class invoker2 : public invoker_base {
 public:
  typedef pfi::lang::function<R(A1, A2)> func_type;

  explicit invoker2(const func_type& f)
      : f_(f) {
  }
  virtual void invoke(msgpack::rpc::request& req) {
    msgpack::type::tuple<A1, A2> params;
    req.params().convert(&params);
    R retval = f_(params.template get<0>(), params.template get<1>());
    req.result<R>(retval);
  }

 private:
  func_type f_;
};

template<typename R, typename A1, typename A2>
pfi::lang::shared_ptr<invoker_base> make_invoker(
    const pfi::lang::function<R(A1, A2)>& f) {
  return pfi::lang::shared_ptr<invoker_base>(new invoker2<R, A1, A2>(f));
}

template<typename R, typename A1, typename A2, typename A3>
class invoker3 : public invoker_base {
 public:
  typedef pfi::lang::function<R(A1, A2, A3)> func_type;

  explicit invoker3(const func_type& f)
      : f_(f) {
  }
  virtual void invoke(msgpack::rpc::request& req) {
    msgpack::type::tuple<A1, A2, A3> params;
    req.params().convert(&params);
    R retval = f_(
        params.template get<0>(),
        params.template get<1>(),
        params.template get<2>());
    req.result<R>(retval);
  }

 private:
  func_type f_;
};

template<typename R, typename A1, typename A2, typename A3>
pfi::lang::shared_ptr<invoker_base> make_invoker(
    const pfi::lang::function<R(A1, A2, A3)>& f) {
  return pfi::lang::shared_ptr<invoker_base>(new invoker3<R, A1, A2, A3>(f));
}

template<typename R, typename A1, typename A2, typename A3, typename A4>
class invoker4 : public invoker_base {
 public:
  typedef pfi::lang::function<R(A1, A2, A3, A4)> func_type;

  explicit invoker4(const func_type& f)
      : f_(f) {
  }
  virtual void invoke(msgpack::rpc::request& req) {
    msgpack::type::tuple<A1, A2, A3, A4> params;
    req.params().convert(&params);
    R retval = f_(
        params.template get<0>(),
        params.template get<1>(),
        params.template get<2>(),
        params.template get<3>());
    req.result<R>(retval);
  }

 private:
  func_type f_;
};

template<typename R, typename A1, typename A2, typename A3, typename A4>
pfi::lang::shared_ptr<invoker_base> make_invoker(
    const pfi::lang::function<R(A1, A2, A3, A4)>& f) {
  return pfi::lang::shared_ptr<invoker_base>(
      new invoker4<R, A1, A2, A3, A4>(f));
}

//
// asynchronous var-arg method invoker
//
//   NOTE 1: var-arg method always receive its arguments as an
//   packed tuple. Thus, we need define only arity-1 invoker.
//
//   NOTE 2: given function to async_vmethod_invoker1 requires
//   two arguments ( request and method argument tuple )
//
template<typename Tuple>
class async_vmethod_invoker1 : public invoker_base {
 public:
  typedef typename async_vmethod<Tuple>::type func_type;

  explicit async_vmethod_invoker1(const func_type& f)
      : f_(f) {
  }
  virtual void invoke(request_type& req) {
    Tuple params;
    req.params().convert(&params);
    (void)f_(req, params);
  }

 private:
  func_type f_;
};

template<typename Tuple>
pfi::lang::shared_ptr<invoker_base> make_async_vmethod_invoker(
    const typename async_vmethod<Tuple>::type& f) {
  return pfi::lang::shared_ptr<invoker_base>(
      new async_vmethod_invoker1<Tuple>(f));
}

// shortcut
template<typename T>
void rpc_server::add(
    const std::string& name,
    const pfi::lang::function<T>& f) {
  add_inner(name, make_invoker(f));
}

template<typename Tuple>
void rpc_server::add_async_vmethod(
    const std::string& name,
    const typename async_vmethod<Tuple>::type& f) {
  add_inner(name, make_async_vmethod_invoker<Tuple>(f));
}

}  // namespace mprpc
}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_MPRPC_RPC_SERVER_HPP_
