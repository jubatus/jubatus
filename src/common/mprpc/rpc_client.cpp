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

#include "rpc_client.hpp"
#include <utility>
#include <glog/logging.h>


using pfi::lang::shared_ptr;
using pfi::system::time::clock_time;
using pfi::system::time::get_clock_time;

namespace jubatus { namespace common { namespace mprpc {

rpc_mclient::rpc_mclient(const std::vector<std::pair<std::string, uint16_t> >& hosts,
                         int timeout_sec):
  hosts_(hosts),
  timeout_sec_(timeout_sec),
  start_(get_clock_time()),
  evbase_(::event_base_new())
{
  connect_async_();
}

rpc_mclient::rpc_mclient(const std::vector<std::pair<std::string, int> >& hosts,
                         int timeout_sec):
  timeout_sec_(timeout_sec),
  start_(get_clock_time()),
  evbase_(::event_base_new())
{
  for(size_t i=0; i<hosts.size(); ++i){
    hosts_.push_back(hosts[i]);
  }
  connect_async_();
}

rpc_mclient::~rpc_mclient(){
  ::event_base_free(evbase_);
}

void rpc_mclient::call_async(const std::string& m)
{
  call_async_(m, std::vector<int>());
}

void rpc_mclient::connect_async_()
{
  clients_.clear();
  for(size_t i=0; i<hosts_.size(); ++i){
    shared_ptr<async_sock> p(new async_sock);
    p->connect_async(hosts_[i].first, hosts_[i].second);
    clients_.insert(std::make_pair(p->get(), socket_context(p)));
  }
}

static void readable_callback(int fd, short int events, void* arg)
{
  async_context* ctx = static_cast<async_context*>(arg);
  try {
    ctx->rest -= ctx->c->readable_callback(fd, events, ctx);
  } catch (...) {
    event_base_loopbreak(ctx->evbase);
  }
}

int rpc_mclient::readable_callback(int fd, int events, async_context* ctx){
  int done = 0;
  pfi::lang::shared_ptr<async_sock> client = clients_[fd].socket;

  if (events == EV_READ) {
    int r = client->recv_async();
    if (r <= 0) {
      if (errno != EAGAIN || errno != EWOULDBLOCK) {
        client->disconnected();
        done++;
      }

    } else {
      typedef msgpack::type::tuple<uint8_t,uint32_t,msgpack::object,msgpack::object> response_t;
      response_t res;

      if(client->salvage<response_t>(res)){
        // cout << __FILE__ << " " << __LINE__ << ":"<< endl;
        // cout << "\ta0: "<< int(res.a0) << endl;
        // cout << "\ta2: "<< res.a2.type << " " << res.a2.is_nil() << " " << res.a2 << endl;
        // cout << "\ta3: "<< res.a3.type << " " << res.a3.is_nil() << " " << res.a3 << endl;;
        done++;
        if(res.a0 == 1){
          if(res.a2.is_nil()){
            ctx->ret.push_back(res.a3);
          }
        }
      }
    }

  } else {
    // EV_TIMEOUT or error occured
    if (events == EV_TIMEOUT)
      ;// TODO: push timeout exception

    client->disconnected();
    done++;
  }

  if (done)
    event_del(&clients_[fd].ev_read);

  if (client->is_closed())
    clients_.erase(fd);

  return done;
}

static void writable_callback(int fd, short int events, void* arg)
{
  async_context* ctx = static_cast<async_context*>(arg);
  try {
    ctx->rest -= ctx->c->writable_callback(fd, events, ctx);
  } catch (...) {
    event_base_loopbreak(ctx->evbase);
  }
}

int rpc_mclient::writable_callback(int fd, int events, async_context* ctx){
  int done = 0;
  pfi::lang::shared_ptr<async_sock> client = clients_[fd].socket;

  if (events == EV_WRITE) {
    if(client->is_connecting()){
      client->set_sending();
    }

    int r = client->send_async(ctx->buf->data(), ctx->buf->size());
    if (r <= 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      client->disconnected();
      done++;

    } else if (client->received() == ctx->buf->size()){
      done++;
      client->reset_received();
      client->set_recving();

    }

  } else {
    // EV_TIMEOUT or error occured
    if (events == EV_TIMEOUT)
      ;// TODO: push timeout exception

    clients_[fd].socket->disconnected();
    done++;
  }

  if (done)
    event_del(&clients_[fd].ev_write);

  if (client->is_closed())
    clients_.erase(fd);

  return done;
}

void rpc_mclient::create_fd_event_()
{
  socket_info_map_t::iterator it, end;
  for (it = clients_.begin(), end = clients_.end(); it != end; ++it) {
    socket_context& c = it->second;

    // use `event_new' when only support libevent 2.x
    event_set(&c.ev_read, c.socket->get(), EV_READ | EV_TIMEOUT | EV_PERSIST, &mprpc::readable_callback, &context_);
    event_set(&c.ev_write, c.socket->get(), EV_WRITE | EV_TIMEOUT | EV_PERSIST, &mprpc::writable_callback, &context_);
    event_base_set(evbase_, &c.ev_read);
    event_base_set(evbase_, &c.ev_write);
  }
}

void rpc_mclient::register_fd_readable_()
{
  struct timeval timeout;
  timeout.tv_sec = timeout_sec_;
  timeout.tv_usec = 0;

  socket_info_map_t::iterator it, end;
  for (it = clients_.begin(), end = clients_.end(); it != end; ++it) {
    event_add(&it->second.ev_read, &timeout);
  }
}

void rpc_mclient::register_fd_writable_()
{
  struct timeval timeout;
  timeout.tv_sec = timeout_sec_;
  timeout.tv_usec = 0;

  socket_info_map_t::iterator it, end;
  for (it = clients_.begin(), end = clients_.end(); it != end; ++it) {
    event_add(&it->second.ev_write, &timeout);
  }
}

void rpc_mclient::send_async(const msgpack::sbuffer& buf)
{
  context_ = async_context();
  context_.c = this;
  context_.evbase = evbase_;
  context_.rest = clients_.size();
  context_.buf = &buf;

  create_fd_event_();
  register_fd_writable_();

  event_base_dispatch(evbase_);
  // FIXME: It is desirable to event_del all write event for preparing failure.
  // TODO: check timeout or connection error
}


void rpc_mclient::join_some_()
{
  context_.ret.clear();

  event_base_dispatch(evbase_);
  // FIXME: It is desirable to event_del all read event for preparing failure.
  // TODO: check timeout or connection error
}

}
}
}

