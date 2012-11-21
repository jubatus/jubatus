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
#include <map>

#include <msgpack.hpp>
#include <event.h>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/function.h>
#include <pficommon/system/time_util.h>
#include <pficommon/network/mprpc/socket.h>
#include <pficommon/network/mprpc/exception.h>

#include "../exception.hpp"
#include "exception.hpp"

namespace jubatus { namespace common { namespace mprpc {

struct rpc_response_t {
  msgpack::type::tuple<uint8_t,uint32_t,msgpack::object,msgpack::object> response;
  pfi::lang::shared_ptr<msgpack::zone> zone;

  bool has_error() const { return !response.a2.is_nil(); }

  uint32_t msgid() const { return response.a1; }
  msgpack::object& error() { return response.a2; }
  template<typename T> const T as() const { return response.a3.as<T>(); }

  rpc_response_t() {}
  rpc_response_t( uint32_t _msgid, const msgpack::object &_error, const msgpack::object &_result,
                  pfi::lang::shared_ptr<msgpack::zone> _zone ) :
    response( /* msgpack::rpc::RESPONSE */ 1, _msgid, _error, _result ),
    zone(_zone) {}
};

class async_sock : public pfi::network::mprpc::socket {
public:
  async_sock();
  ~async_sock();
  int send_async(const char* buf, size_t size);

  int recv_async();

  template <typename T>
  bool salvage(T&, pfi::lang::shared_ptr<msgpack::zone>&);

  int connect_async(const std::string& host, uint16_t port);
  bool close();

  void set_online() { state_ = ONLINE; }
  void disconnected() { this->close(); state_ = CLOSED; }
  bool is_closed() const { return state_ == CLOSED; }
  bool is_connecting() const{ return state_ == CONNECTING; }
  bool is_online() const { return state_ == ONLINE; }
  size_t sent_size() const { return progress_; }
  void reset_progress() { progress_ = 0; }

private:
  enum { CLOSED, CONNECTING, ONLINE } state_;
  size_t progress_;
  msgpack::unpacker unpacker_;
};

template <typename T>
bool async_sock::salvage(T& t, pfi::lang::shared_ptr<msgpack::zone>& ret_z)
{
  msgpack::unpacked msg;
  try {
    if (unpacker_.next(&msg)) {
      msgpack::object o = msg.get();
      std::auto_ptr<msgpack::zone> z = msg.zone();
      ret_z = pfi::lang::shared_ptr<msgpack::zone>(z.get());
      z.release();
      o.convert(&t);
      return true;
    }
  } catch (const msgpack::type_error&) {
    throw JUBATUS_EXCEPTION(rpc_internal_error()
        << jubatus::exception::error_message("msgpack::type_error: cannot convert RPC response"));
  } catch (const msgpack::unpack_error& e) {
    throw JUBATUS_EXCEPTION(rpc_internal_error()
        << jubatus::exception::error_message(std::string("msgpack::unpack_error: ")  + e.what()));
  }
  return false;
}

class rpc_mclient;

class async_client {
  friend class rpc_mclient;
public:
  typedef std::vector<rpc_response_t> response_list_t;

  async_client(const std::string& host, uint16_t port);
  ~async_client();

  void connect_async();

  void send_async(const std::string& method, const msgpack::sbuffer& argv);
  void send_async(const msgpack::sbuffer& argv);
  void join(msgpack::object& o);

  void prepare_event(event_base* evbase);

  void set_send_buffer(const msgpack::sbuffer&);

  void register_read(const timeval&);
  void register_write(const timeval&);

  std::string host() const { return host_; }
  uint16_t port() const { return port_; }
  pfi::lang::shared_ptr<async_sock> sock() const { return sock_; }
  event_base* evbase() const { return evbase_; }
  event* read_ev() const { return &context_->ev_read; }
  event* write_ev() const { return &context_->ev_write; }
  jubatus::exception::exception_thrower_ptr read_exception() const { return context_->read_exception; }
  jubatus::exception::exception_thrower_ptr write_exception() const { return context_->write_exception; }

  response_list_t& response() { return results_; }

  bool is_closed() const { return sock_->is_closed(); }

private:

  struct client_context {
    event_base* evbase;
    async_client* client;
    event ev_read;
    event ev_write;
    jubatus::exception::exception_thrower_ptr read_exception;
    jubatus::exception::exception_thrower_ptr write_exception;

    client_context()
    {
      evbase = NULL;
      client = NULL;
      memset(&ev_read, 0, sizeof(event));
      memset(&ev_write, 0, sizeof(event));
    }
  };

  static void readable_callback_cb(int fd, short int events, void* arg) throw();
  static void writable_callback_cb(int fd, short int events, void* arg) throw();

  void readable_callback(int, int);
  void writable_callback(int, int);

  void prepare_event();

  std::string host_;
  uint16_t port_;

  pfi::lang::shared_ptr<async_sock> sock_;
  response_list_t results_;

  pfi::lang::shared_ptr<client_context> context_;
  event_base* evbase_;
  const mutable msgpack::sbuffer* send_buffer_; // rpc sending buffer
  bool clear_evbase_;
};

} // mprpc
} // common
} // jubatus
