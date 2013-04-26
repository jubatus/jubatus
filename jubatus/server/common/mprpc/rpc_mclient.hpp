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

#ifndef JUBATUS_SERVER_COMMON_MPRPC_RPC_MCLIENT_HPP_
#define JUBATUS_SERVER_COMMON_MPRPC_RPC_MCLIENT_HPP_

#include <stdint.h>
#include <vector>
#include <string>
#include <utility>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/function.h>
#include <pficommon/lang/noncopyable.h>

#include <msgpack.hpp>
#include <jubatus/msgpack/rpc/client.h>
#include <jubatus/msgpack/rpc/session_pool.h>

#include "rpc_error.hpp"
#include "rpc_result.hpp"

#define JUBATUS_MSGPACKRPC_EXCEPTION_DEFAULT_HANDLER(method) \
  catch (msgpack::rpc::no_method_error) { \
    throw JUBATUS_EXCEPTION( \
        jubatus::server::common::mprpc::rpc_method_not_found() \
        << jubatus::server::common::mprpc::error_method(method)); \
  } catch (msgpack::rpc::argument_error) { \
    throw JUBATUS_EXCEPTION( \
        jubatus::server::common::mprpc::rpc_type_error() \
        << jubatus::server::common::mprpc::error_method(method)); \
  } catch (msgpack::rpc::remote_error& e) { \
                                                                        \
    /* NOTE:                                                         */ \
    /*   msgpack-rpc raise remote_error against 'integer'-type error */ \
    /* ( excluding NO_METHOD_ERROR and ARGUMENT_ERROR ) and          */ \
    /* non-integer error ( excluding TIMEOUT_ERROR, CONNECT_ERROR ). */ \
    /* We map these errors to                                        */ \
    /* juba's rpc_call_error with error code or error message.       */ \
                                                                        \
    msgpack::object err = e.error(); \
    if (err.type == msgpack::type::POSITIVE_INTEGER) { \
      throw JUBATUS_EXCEPTION( \
          jubatus::server::common::mprpc::rpc_call_error() \
          << jubatus::server::common::mprpc::error_method(method) \
          << jubatus::exception::error_message( \
                 std::string("rpc_server error: " \
                     + pfi::lang::lexical_cast<std::string>(err.via.u64)))); \
    } else { \
      throw JUBATUS_EXCEPTION( \
          jubatus::server::common::mprpc::rpc_call_error() \
          << jubatus::server::common::mprpc::error_method(method) \
          << jubatus::exception::error_message( \
                 std::string("rpc_server error: " \
                     + pfi::lang::lexical_cast<std::string>(err)))); \
    } \
  } catch(msgpack::rpc::connect_error) { \
    throw JUBATUS_EXCEPTION( \
        jubatus::server::common::mprpc::rpc_io_error() \
        << jubatus::server::common::mprpc::error_method(method)); \
  } catch(msgpack::rpc::timeout_error) { \
    throw JUBATUS_EXCEPTION( \
        jubatus::server::common::mprpc::rpc_timeout_error() \
        << jubatus::server::common::mprpc::error_method(method)); \
  } catch(msgpack::type_error) { \
                                                                     \
    /* NOTE: msgpack-rpc will raise msgpack::type_error exception */ \
    /* against broken messages. We map these errors to juba's     */ \
    /* rpc_no_result.                                             */ \
    /*                                                            */ \
    /* NOTE: juba's rpc_type_errors are preffered. but these are  */ \
    /* used in the sense of "rpc method argument mismatch".       */ \
    /* So that, new exception class is expected like              */ \
    /* rcp_broken_message, ...                                    */ \
                                                                     \
    throw JUBATUS_EXCEPTION( \
        jubatus::server::common::mprpc::rpc_no_result() \
        << jubatus::server::common::mprpc::error_method(method)); \
  }

namespace jubatus {
namespace server {
namespace common {
namespace mprpc {

class rpc_mclient : pfi::lang::noncopyable {
 public:
  typedef std::vector<std::pair<std::string, uint16_t> > host_spec_list_t;

  rpc_mclient(
      const host_spec_list_t& hosts,
      int timeout_sec,
      msgpack::rpc::session_pool* pool = NULL)
      : hosts_(hosts),
        timeout_sec_(timeout_sec),
        pool_(NULL),
        pool_allocated_(false) {
    init_pool(pool);
  }

  rpc_mclient(
      const std::vector<std::pair<std::string, int> >& hosts,
      int timeout_sec,
      msgpack::rpc::session_pool* pool = NULL)
      : timeout_sec_(timeout_sec),
        pool_(NULL),
        pool_allocated_(false) {
    hosts_.reserve(hosts.size());
    for (size_t i = 0; i < hosts.size(); ++i) {
      hosts_.push_back(hosts[i]);
    }
    init_pool(pool);
  }

  ~rpc_mclient() {
    if (pool_allocated_ && pool_) {
      delete pool_;
      pool_ = NULL;
    }
  }

  template<typename Res, typename A0>
  rpc_result<Res> call(
      const std::string& m,
      const A0& a0,
      const pfi::lang::function<Res(Res, Res)>& reducer);
  template<typename Res, typename A0, typename A1>
  rpc_result<Res> call(
      const std::string& m,
      const A0& a0,
      const A1& a1,
      const pfi::lang::function<Res(Res, Res)>& reducer);

  template<typename Res, typename A0, typename A1, typename A2>
  rpc_result<Res> call(
      const std::string& m,
      const A0& a0,
      const A1& a1,
      const A2& a2,
      const pfi::lang::function<Res(Res, Res)>& reducer);
  template<typename Res, typename A0, typename A1, typename A2, typename A3>
  rpc_result<Res> call(
      const std::string& m,
      const A0& a0,
      const A1& a1,
      const A2& a2,
      const A3& a3,
      const pfi::lang::function<Res(Res, Res)>& reducer);

  template<typename A0>
  rpc_result_object call(const std::string&, const A0& a0);

 private:
  void init_pool(msgpack::rpc::session_pool* pool) {
    if (pool) {
      pool_ = pool;
    } else {
      pool_ = new msgpack::rpc::session_pool();
      pool_allocated_ = true;
    }
  }

  template<typename Args>
  void call_(const std::string& m, const Args& args);
  template<typename Res>
  rpc_result<Res> join_(
      const std::string& method,
      const pfi::lang::function<Res(Res, Res)>& reducer);
  template<typename Res>
  void join_one_(
      const std::string& method,
      msgpack::rpc::future& response,
      rpc_result<Res>& result,
      const pfi::lang::function<Res(Res, Res)>& reducer);

  rpc_result_object wait(const std::string& method);
  rpc_response_t wait_one(const std::string& method, msgpack::rpc::future& f);

  host_spec_list_t hosts_;
  int timeout_sec_;

  msgpack::rpc::session_pool* pool_;
  bool pool_allocated_;
  std::vector<msgpack::rpc::future> futures_;
};

template<typename Res, typename A0>
rpc_result<Res> rpc_mclient::call(
    const std::string& m,
    const A0& a0,
    const pfi::lang::function<Res(Res, Res)>& reducer) {
  call_(m, msgpack::type::tuple<const A0&>(a0));
  return join_(m, reducer);
}

template<typename Res, typename A0, typename A1>
rpc_result<Res> rpc_mclient::call(
    const std::string& m,
    const A0& a0,
    const A1& a1,
    const pfi::lang::function<Res(Res, Res)>& reducer) {
  call_(m, msgpack::type::tuple<const A0&, const A1&>(a0, a1));
  return join_(m, reducer);
}

template<typename Res, typename A0, typename A1, typename A2>
rpc_result<Res> rpc_mclient::call(
    const std::string& m,
    const A0& a0,
    const A1& a1,
    const A2& a2,
    const pfi::lang::function<Res(Res, Res)>& reducer) {
  call_(m, msgpack::type::tuple<const A0&, const A1&, const A2&>(a0, a1, a2));
  return join_(m, reducer);
}

template<typename Res, typename A0, typename A1, typename A2, typename A3>
rpc_result<Res> rpc_mclient::call(
    const std::string& m,
    const A0& a0,
    const A1& a1,
    const A2& a2,
    const A3& a3,
    const pfi::lang::function<Res(Res, Res)>& reducer) {
  call_(
      m,
      msgpack::type::tuple<const A0&, const A1&, const A2&, const A3&>(
          a0,
          a1,
          a2,
          a3));
  return join_(m, reducer);
}

template<typename Args>
void rpc_mclient::call_(const std::string& m, const Args& args) {
  futures_.clear();
  futures_.reserve(hosts_.size());
  for (host_spec_list_t::iterator itr = hosts_.begin(), end = hosts_.end();
      itr != end; ++itr) {
    msgpack::rpc::session s = pool_->get_session(itr->first, itr->second);
    s.set_timeout(timeout_sec_);
    futures_.push_back(s.call_apply(m, args));
  }
}

template<typename Res>
void rpc_mclient::join_one_(
    const std::string& method,
    msgpack::rpc::future& response,
    rpc_result<Res>& result,
    const pfi::lang::function<Res(Res, Res)>& reducer) {
  try {
    Res method_result = response.get<Res>();
    if (result.value) {
      *result.value = reducer(*result, method_result);
    } else {
      result.value = pfi::lang::shared_ptr<Res>(new Res(method_result));
    }
  }
  JUBATUS_MSGPACKRPC_EXCEPTION_DEFAULT_HANDLER(method);
}

template<typename Res>
rpc_result<Res> rpc_mclient::join_(
    const std::string& method,
    const pfi::lang::function<Res(Res, Res)>& reducer) {
  rpc_result<Res> result;

  if (futures_.empty()) {
    throw JUBATUS_EXCEPTION(rpc_no_client() << error_method(method));
  }

  size_t result_count = 0;
  for (size_t i = 0; i < futures_.size(); ++i) {
    try {
      join_one_(method, futures_[i], result, reducer);
      ++result_count;
    } catch (...) {
      // continue process next result when exception thrown.
      // store exception_thrower to list of errors

      result.error.push_back(
          rpc_error(hosts_[i].first, hosts_[i].second,
              jubatus::exception::get_current_exception()));
    }
  }

  if (result_count == 0) {
    rpc_no_result e;
    if (result.has_error()) {
      e << error_multi_rpc(result.error);
    }
    throw JUBATUS_EXCEPTION(e << error_method(method));
  }

  return result;
}

template<typename A0>
rpc_result_object rpc_mclient::call(const std::string& m, const A0& a0) {
  call_(m, msgpack::type::tuple<const A0&>(a0));
  return wait(m);
}

}  // namespace mprpc
}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_MPRPC_RPC_MCLIENT_HPP_
