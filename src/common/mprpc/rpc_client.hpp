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
#include <event.h>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/function.h>
#include <glog/logging.h>
#include <pficommon/data/unordered_map.h>
#include <pficommon/data/string/utility.h>
#include <pficommon/lang/noncopyable.h>
#include <pficommon/network/mprpc/message.h>

#include "../exception.hpp"
#include "exception.hpp"
#include "async_client.hpp"

namespace jubatus { namespace common { namespace mprpc {

class rpc_mclient;

class rpc_error {
public:
  rpc_error(const std::string& host, uint16_t port)
    : host_(host), port_(port)
  {}

  rpc_error(const std::string& host, uint16_t port, jubatus::exception::exception_thrower_ptr thrower)
    : host_(host), port_(port), exception_(thrower)
  {}

  std::string host() const { return host_; }
  uint16_t port() const { return port_; }

  bool has_exception() const { return exception_; }
  void throw_exception() const { exception_->throw_exception(); }
  jubatus::exception::exception_thrower_ptr exception() const { return exception_; }

private:
  std::string host_;
  uint16_t port_;
  jubatus::exception::exception_thrower_ptr exception_;
};

typedef jubatus::exception::error_info<struct error_multi_rpc_, std::vector<rpc_error> > error_multi_rpc;
inline std::string to_string(const error_multi_rpc& info)
{
  std::ostringstream result;

  size_t host_size = info.value().size();
  if (host_size == 1) {
    result << "rpc_error with 1 server" << '\n';
  } else if (host_size > 1) {
    result << "rpc_error with " << host_size << " servers" << '\n';
  }

  std::vector<rpc_error> errors = info.value();
  for (std::vector<rpc_error>::const_iterator it = errors.begin(), end = errors.end();
      it != end; ++it) {
    result << " host: " << it->host() << ", port: " << it->port() << '\n';
    std::ostringstream tmp;
    if (it->has_exception()) {
      try {
        it->throw_exception();
      } catch (const jubatus::exception::jubatus_exception& e) {
        tmp << e.diagnostic_information(false);
      } catch (const std::exception& e) {
        tmp << e.what();
      } catch (...) {
        tmp << "...";
      }

      // Indent each line
      std::vector<std::string> lines = pfi::data::string::split(tmp.str(), '\n');
      std::ostringstream msg;
      for (std::vector<std::string>::iterator it = lines.begin(), end = lines.end();
          it != end; ++it) {
        if (it->empty()) continue;
        msg << "   " << *it << '\n';
      }
      result << msg.str();
    }
  }

  return result.str();
}

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

class rpc_mclient : pfi::lang::noncopyable
{
public:
  rpc_mclient(const std::vector<std::pair<std::string, uint16_t> >& hosts,
              int timeout_sec);
  rpc_mclient(const std::vector<std::pair<std::string, int> >& hosts,
              int timeout_sec);
  ~rpc_mclient();

  template <typename Res, typename A0>
  rpc_result<Res> call(const std::string&, const A0& a0, const pfi::lang::function<Res(Res,Res)>& reducer);
  template <typename Res, typename A0, typename A1>
  rpc_result<Res> call(const std::string&, const A0& a0, const A1& a1, const pfi::lang::function<Res(Res,Res)>& reducer);
  template <typename Res, typename A0, typename A1, typename A2>
  rpc_result<Res> call(const std::string&, const A0& a0, const A1& a1, const A2& a2, const pfi::lang::function<Res(Res,Res)>& reducer);
  template <typename Res, typename A0, typename A1, typename A2, typename A3>
  rpc_result<Res> call(const std::string&, const A0&, const A1&, const A2&, const A3&, const pfi::lang::function<Res(Res,Res)>& reducer);

  template <typename A0>
  rpc_result_object call(const std::string&, const A0& a0);

private:
  static void readable_callback(int fd, short int events, void* arg);
  static void writable_callback(int fd, short int events, void* arg);

  void connect_async_();

  void create_fd_event_();
  void register_fd_readable_();
  void register_fd_writable_(const msgpack::sbuffer&);

  void send_all(const msgpack::sbuffer& buf);

  rpc_result_object wait(const std::string& m);

  template <typename Arr>
  void call_(const std::string&, const Arr& a);

  template <typename Res>
  rpc_result<Res> join_(const std::string&, const pfi::lang::function<Res(Res,Res)>& reducer);

  std::vector<std::pair<std::string, uint16_t> > hosts_;
  int timeout_sec_;

  typedef std::vector<pfi::lang::shared_ptr<async_client> > client_list_t;
  client_list_t clients_;

  pfi::system::time::clock_time start_;

  event_base* evbase_;
};

template <typename Arr>
void rpc_mclient::call_(const std::string& m, const Arr& argv)
{
  msgpack::sbuffer sbuf;
  msgpack::type::tuple<uint8_t,uint32_t,std::string,Arr> rpc_request(0, 0xDEADBEEF, m, argv);
  msgpack::pack(&sbuf, rpc_request);
  send_all(sbuf);
}

template <typename Res>
rpc_result<Res> rpc_mclient::join_(const std::string& method, const pfi::lang::function<Res(Res,Res)>& reducer)
{
  using pfi::lang::shared_ptr;
  rpc_result<Res> result;

  if (clients_.empty())
    throw JUBATUS_EXCEPTION(rpc_no_client() << error_method(method));

  register_fd_readable_();
  event_base_dispatch(evbase_);

  size_t count = 0;

  for (client_list_t::iterator it = clients_.begin(), end = clients_.end();
      it != end; ++it) {
    shared_ptr<async_client> client = *it;
    async_client::response_list_t& response_list = client->response();

    try {
      if (client->read_exception())
        client->read_exception()->throw_exception();
      if (client->write_exception())
        client->write_exception()->throw_exception();
      if (response_list.empty())
        throw JUBATUS_EXCEPTION(rpc_no_result() << error_method(method));

      // If implement RPC specification strictly, you must find response by msgid,
      // but pfi::network::mprpc::rpc_server does not support RPC pipelining.
      rpc_response_t res = response_list.front();
      response_list.erase(response_list.begin());
      if (res.has_error()) {
        if (res.error().type == msgpack::type::POSITIVE_INTEGER) {
          // error code defined in pficommon/mprpc/message.h
          switch (static_cast<unsigned int>(res.error().via.u64)) {
          case pfi::network::mprpc::METHOD_NOT_FOUND:
            throw JUBATUS_EXCEPTION(rpc_method_not_found() << error_method(method));

          case pfi::network::mprpc::TYPE_MISMATCH:
            throw JUBATUS_EXCEPTION(rpc_type_error() << error_method(method));

          default:
            throw JUBATUS_EXCEPTION(rpc_call_error()
                << error_method(method)
                << jubatus::exception::error_message(std::string("rpc_server error: " + pfi::lang::lexical_cast<std::string>(res.error().via.u64))));
          }
        } else {
          // MEMO: other error object returned
          throw JUBATUS_EXCEPTION(rpc_call_error()
              << error_method(method)
              << jubatus::exception::error_message("error response: " + pfi::lang::lexical_cast<std::string>(res.error())));
        }
      }

      try {
        // MEMO: msgpack can be thrown an exception
        if (result.value) {
          *result.value = reducer(*result, res.as<Res>());
        } else {
          result.value = shared_ptr<Res>(new Res(res.as<Res>()));
        }
      } catch (const msgpack::type_error&) {
          throw JUBATUS_EXCEPTION(rpc_type_error()
              << error_method(method)
              << jubatus::exception::error_message("recv object cannot convert"));
      }

      count++;

    // continue process next result when exception thrown
    } catch (...) {
      // store exception_thrower to list of error
      result.error.push_back(rpc_error(client->host(), client->port(), jubatus::exception::get_current_exception()));

      // clear last exception set by libevent callback
      client->context_->read_exception.reset();
      client->context_->write_exception.reset();
    }
  }

  if (!count) {
    rpc_no_result e;
    if (result.has_error())
      e << error_multi_rpc(result.error);
    throw JUBATUS_EXCEPTION(e << error_method(method));
  }

  return result;
}

template <typename Res, typename A0>
rpc_result<Res> rpc_mclient::call(const std::string& m, const A0& a0, const pfi::lang::function<Res(Res,Res)>& reducer)
{
  call_(m, msgpack::type::tuple<A0>(a0));
  return join_(m, reducer);
}

template <typename Res, typename A0, typename A1>
rpc_result<Res> rpc_mclient::call(const std::string& m, const A0& a0, const A1& a1, const pfi::lang::function<Res(Res,Res)>& reducer)
{
  call_(m, msgpack::type::tuple<A0, A1>(a0, a1));
  return join_(m, reducer);
}

template <typename Res, typename A0, typename A1, typename A2>
rpc_result<Res> rpc_mclient::call(const std::string& m, const A0& a0, const A1& a1, const A2& a2, const pfi::lang::function<Res(Res,Res)>& reducer)
{
  call_(m, msgpack::type::tuple<A0, A1, A2>(a0, a1, a2));
  return join_(m, reducer);
}

template <typename Res, typename A0, typename A1, typename A2, typename A3>
rpc_result<Res> rpc_mclient::call(const std::string& m, const A0& a0, const A1& a1, const A2& a2, const A3& a3, const pfi::lang::function<Res(Res,Res)>& reducer)
{
  call_(m, msgpack::type::tuple<A0, A1, A2, A3>(a0, a1, a2, a3));
  return join_(m, reducer);
}



template <typename A0>
rpc_result_object rpc_mclient::call(const std::string& m, const A0& a0)
{
  call_(m, msgpack::type::tuple<A0>(a0));
  return wait(m);
}

} // mprpc
} // common
} // jubatus
