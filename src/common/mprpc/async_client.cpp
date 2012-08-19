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

#include "async_client.hpp"
#include <pficommon/network/mprpc/socket.h>
#include <pficommon/network/mprpc/exception.h>
#include <pficommon/network/ipv4.h>

#include <glog/logging.h>

#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "exception.hpp"

using pfi::lang::shared_ptr;
using pfi::system::time::clock_time;
using pfi::system::time::get_clock_time;

namespace jubatus { namespace common { namespace mprpc {

bool set_socket_nonblock(int sock, bool on)
{
  int res;
  if(on){
    res = fcntl(sock, F_SETFL, O_NONBLOCK);
    return res==0;
  }else{
    //FIXME
    return false;
  }
}

async_sock::async_sock():
  pfi::network::mprpc::socket(::socket(AF_INET, SOCK_STREAM, 0)),
  state_(CLOSED),
  progress_(0)
{
  // FIXME: SOCK_NONBLOCK is linux only
  if (!set_socket_nonblock(this->get(), true))
    throw JUBATUS_EXCEPTION(rpc_internal_error()
        << jubatus::exception::error_api_func("fcntl")
        << jubatus::exception::error_errno(errno));

  unpacker_.reserve_buffer(4096);
}

async_sock::~async_sock()
{
  this->close();
}

int async_sock::send_async(const char* buf, size_t size)
{
  int r = ::write(this->get(), buf + progress_, size - progress_);
  if (r > 0)
    progress_ += r;

  return r;
}

int async_sock::recv_async()
{
  if (unpacker_.buffer_capacity() == 0)
    unpacker_.reserve_buffer(4096);

  int r = ::read(this->get(), unpacker_.buffer(), unpacker_.buffer_capacity());
  if (r > 0)
    unpacker_.buffer_consumed(r);

  return r;
}

int async_sock::connect_async(const std::string& host, uint16_t port)
{
  int res;
  int sock = this->get();

  std::vector<pfi::network::ipv4_address> ips = resolve(host, port);
  for (size_t i = 0, size = ips.size(); i < size; i++) {
    sockaddr_in addrin={};
    addrin.sin_family = PF_INET;
    addrin.sin_addr.s_addr = inet_addr(ips[i].to_string().c_str());
    addrin.sin_port = htons(port);

    res = ::connect(sock,(sockaddr*)&addrin,sizeof(addrin));
    if (res == -1) {
      if (errno == EINPROGRESS) {
        state_ = CONNECTING;
        return 0;
      } else {
        DLOG(ERROR) << __func__ << " failed to connect: " << errno;
      }
    } else if (res == 0) {
      state_ = ONLINE;
      return 0;
    }
  }
  this->close();
  return -1;
}

bool async_sock::close()
{
  return pfi::network::mprpc::socket::close();
}

async_client::async_client(const std::string& host, uint16_t port)
  : host_(host)
  , port_(port)
  , sock_(new async_sock)
  , context_(new client_context)
  , evbase_(NULL)
  , send_buffer_(NULL)
  , clear_evbase_(false)
{
  context_->client = this;
}

async_client::~async_client()
{
  if (clear_evbase_ && evbase_)
    event_base_free(evbase_);
}

void async_client::connect_async()
{
  sock_->connect_async(host_, port_);
}

void async_client::prepare_event(event_base* evbase)
{
  context_->evbase = evbase_ = evbase;

  // use `event_new' when only support libevent 2.x
  event_set(&context_->ev_read, sock_->get(), EV_READ | EV_TIMEOUT | EV_PERSIST, &readable_callback_cb, context_.get());
  event_set(&context_->ev_write, sock_->get(), EV_WRITE | EV_TIMEOUT | EV_PERSIST, &writable_callback_cb, context_.get());
  event_base_set(evbase_, &context_->ev_read);
  event_base_set(evbase_, &context_->ev_write);
}

void async_client::prepare_event()
{
  if (!evbase_) {
    clear_evbase_ = true;
    evbase_ = event_base_new();
    if (!evbase_)
      throw std::bad_alloc();

    prepare_event(evbase_);
  }
}

void async_client::set_send_buffer(const msgpack::sbuffer& s)
{
  send_buffer_ = &s;
}

void async_client::register_read(const timeval& timeout)
{
  event_add(&context_->ev_read, &timeout);
}

void async_client::register_write(const timeval& timeout)
{
  event_add(&context_->ev_write, &timeout);
}

void async_client::readable_callback_cb(int fd, short int events, void* arg) throw()
{
  client_context* context = static_cast<client_context*>(arg);
  async_client* client = context->client;
  context->read_exception.reset();

  try {
    client->readable_callback(fd, events);

  } catch (jubatus::exception::jubatus_exception& e) {
    event_del(&context->ev_read);
    client->sock_->disconnected();
    context->read_exception = e.thrower();

    try {
      throw;
    } catch (rpc_internal_error& e) {
      e << jubatus::exception::error_api_func("event_base_dispatch");
    } catch (...) {}

  } catch (...) {
    // unknown error
    event_del(&context->ev_read);
    event_base_loopbreak(context->evbase);
    context->read_exception = jubatus::exception::get_current_exception();
  }
}

void async_client::writable_callback_cb(int fd, short int events, void* arg) throw()
{
  client_context* context = static_cast<client_context*>(arg);
  async_client* client = context->client;
  context->write_exception.reset();

  try {
    client->writable_callback(fd, events);

  } catch (jubatus::exception::jubatus_exception& e) {
    event_del(&context->ev_write);
    client->sock_->disconnected();
    context->write_exception = e.thrower();

    try {
      throw;
    } catch (rpc_internal_error& e) {
      e << jubatus::exception::error_api_func("event_base_dispatch");
    } catch (...) {}

  } catch (...) {
    // unknown error
    event_del(&context->ev_write);
    event_base_loopbreak(context->evbase);
    context->write_exception = jubatus::exception::get_current_exception();
  }
}

void async_client::readable_callback(int fd, int events)
{
  if (events == EV_READ) {
    int r = sock_->recv_async();
    if (r < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      throw JUBATUS_EXCEPTION(rpc_io_error()
          << jubatus::exception::error_api_func("read")
          << jubatus::exception::error_errno(errno));

    } else if (r == 0) {
      // closed socket
      sock_->disconnected();

    } else {
      rpc_response_t res;

      if (sock_->salvage(res.response, res.zone)) {
        //cout << __FILE__ << " " << __LINE__ << ":"<< endl;
        //cout << "\ta0: "<< int(res.response.a0) << endl;
        //cout << "\ta2: "<< res.response.a2.type << " " << res.response.a2.is_nil() << " " << res.response.a2 << endl;
        //cout << "\ta3: "<< res.response.a3.type << " " << res.response.a3.is_nil() << " " << res.response.a3 << endl;;

        // Response received
        event_del(&context_->ev_read);

        // RPC Response Message: [type = 1, msgid, error, result]
        if (res.response.a0 == 1)
          results_.push_back(res);
      }
    }

  } else {
    // EV_TIMEOUT or error occured
    if (events == EV_TIMEOUT)
      throw JUBATUS_EXCEPTION(rpc_timeout_error()
          << jubatus::exception::error_api_func("read")
          << jubatus::exception::error_errno(errno));
    else
      throw JUBATUS_EXCEPTION(rpc_internal_error()
          << jubatus::exception::error_api_func("read")
          << jubatus::exception::error_errno(errno));
  }
}

void async_client::writable_callback(int fd, int events)
{
  if (events == EV_WRITE) {
    if (sock_->is_connecting())
      sock_->set_online();

    int r = sock_->send_async(send_buffer_->data(), send_buffer_->size());
    if (r < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        return;

      // write error
      throw JUBATUS_EXCEPTION(rpc_io_error()
          << jubatus::exception::error_api_func("write")
          << jubatus::exception::error_errno(errno));

    } else if (r == 0) {
      // there is no effect
    } else if (sock_->sent_size() == send_buffer_->size()){
      // current packet sending finished
      event_del(&context_->ev_write);
      sock_->reset_progress();

    }

  } else {
    // EV_TIMEOUT or error occured
    if (events == EV_TIMEOUT)
      throw JUBATUS_EXCEPTION(rpc_timeout_error()
          << jubatus::exception::error_api_func("write")
          << jubatus::exception::error_errno(errno));
    else
      throw JUBATUS_EXCEPTION(rpc_internal_error()
          << jubatus::exception::error_api_func("write")
          << jubatus::exception::error_errno(errno));
  }
}

} // mprpc
} // common
} // jubatus
