// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_COMMON_MPRPC_RPC_UTIL_HPP_
#define JUBATUS_SERVER_COMMON_MPRPC_RPC_UTIL_HPP_

#include <string>
#include <utility>
#include <jubatus/msgpack/rpc/client.h>
#include <pficommon/data/serialization.h>
#include <msgpack.hpp>
#include "rpc_server.hpp"

namespace jubatus {

typedef std::pair<std::string, int> connection_info;

template<typename T, typename E = std::string>
struct result {
  bool success;
  T retval;
  E error;

  static result<T, E> ok(const T& t) {
    result<T, E> r;
    r.success = true;
    r.retval = t;
    return r;
  }

  static result<T, E> fail(const E& e) {
    result<T, E> r;
    r.success = false;
    r.error = e;
    return r;
  }


  MSGPACK_DEFINE(success, retval, error);
  template<class Archiver>
  void serialize(Archiver& ar) {
    ar & MEMBER(success) & MEMBER(retval) & MEMBER(error);
  }
};

#define JUBATUS_MPRPC_PROC(name, ret_type, param_list)                    \
  namespace _server_impl {                                                \
  class name : public virtual jubatus::server::common::mprpc::rpc_server { \
  public:                                                                 \
    name() : rpc_server(0) { }                                            \
    void set_##name(const pfi::lang::function< ret_type param_list> &f) { \
      rpc_server::add< ret_type param_list>(#name, f);                    \
    }                                                                     \
  };                                                                      \
  }                                                                       \
                                                                          \
  namespace _client_impl {                                                \
  class name : public virtual msgpack::rpc::client {                      \
  public:                                                                 \
    name(): msgpack::rpc::client("", 0) { }                               \
                                                                          \
    template<typename A0>                                                 \
    ret_type call_##name(A0 a0) {                                         \
      return call(#name, a0).template get<ret_type>();                    \
    }                                                                     \
    template<typename A0, typename A1>                                    \
    ret_type call_##name(A0 a0, A1 a1) {                                  \
      return call(#name, a0, a1).template get<ret_type>();                \
    }                                                                     \
    template<typename A0, typename A1, typename A2>                       \
    ret_type call_##name(A0 a0, A1 a1, A2 a2) {                           \
      return call(#name, a0, a1, a2).template get<ret_type>();            \
    }                                                                     \
    template<typename A0, typename A1, typename A2, typename A3>          \
    ret_type call_##name(A0 a0, A1 a1, A2 a2, A3 a3) {                    \
      return call(#name, a0, a1, a2, a3).template get<ret_type>();        \
    }                                                                     \
  };                                                                      \
  }

#define JUBATUS_MPRPC_GEN(ver, base, ...)                               \
  namespace _server_impl {                                              \
  struct base##_server : __VA_ARGS__ {                                  \
  public:                                                               \
    base##_server(double timeout_sec)                                   \
    : jubatus::server::common::mprpc::rpc_server(timeout_sec) { }       \
    void rpc_listen(uint16_t port) {                                    \
      jubatus::server::common::mprpc::rpc_server::listen(port);         \
    }                                                                   \
    void rpc_start(int nthreads, bool no_hang = false) {                \
      jubatus::server::common::mprpc::rpc_server::start(nthreads, no_hang); \
    }                                                                   \
    void rpc_join() {                                                   \
      jubatus::server::common::mprpc::rpc_server::join();                \
    }                                                                   \
    void rpc_stop() {                                                   \
      jubatus::server::common::mprpc::rpc_server::stop();               \
    }                                                                   \
    void rpc_close() {                                                  \
      jubatus::server::common::mprpc::rpc_server::close();              \
    }                                                                   \
  };                                                                    \
  }                                                                     \
  typedef _server_impl::base##_server base##_server;                    \
                                                                        \
  namespace _client_impl {                                              \
  struct base##_client : __VA_ARGS__ {                                  \
  public:                                                               \
    base##_client(                                                      \
        const std::string& host,                                        \
        uint16_t port,                                                  \
        double timeout_sec)                                             \
        : msgpack::rpc::client(host, port) {                            \
      set_timeout(timeout_sec);                                         \
    }                                                                   \
  };                                                                    \
  }                                                                     \
  typedef _client_impl::base##_client base##_client;

}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_MPRPC_RPC_UTIL_HPP_
