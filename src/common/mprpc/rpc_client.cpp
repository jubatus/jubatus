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

#include "rpc_client.hpp"
#include <glog/logging.h>

#include <event.h>

using pfi::lang::shared_ptr;
using pfi::system::time::clock_time;
using pfi::system::time::get_clock_time;

namespace jubatus { namespace common { namespace mprpc {

rpc_mclient::rpc_mclient(const std::vector<std::pair<std::string, uint16_t> >& hosts,
                         int timeout_sec):
  hosts_(hosts),
  timeout_sec_(timeout_sec),
  start_(get_clock_time()),
  //  epfd_(epoll_create(64))
  evbase_(::event_base_new())
{
  connect_async_();
}
rpc_mclient::rpc_mclient(const std::vector<std::pair<std::string, int> >& hosts,
                         int timeout_sec):
  timeout_sec_(timeout_sec),
  start_(get_clock_time()),
  //epfd_(epoll_create(64))
  evbase_(::event_base_new())
{
  for(size_t i=0; i<hosts.size(); ++i){
    hosts_.push_back(hosts[i]);
  }
  connect_async_();
}

rpc_mclient::~rpc_mclient(){
  ::event_base_free(evbase_.get());
  evbase_.reset();
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
    clients_[p->get()] = p;
    
    //add_out(epfd_, p->get(), i);
  }
}

static void readable_callback(int fd, short int events, void* arg){
  async_context* ctx = reinterpret_cast<async_context*>(arg);
  ctx->rest -= ctx->c->readable_callback(fd, events, ctx);
}
int rpc_mclient::readable_callback(int fd, int events, async_context* ctx){
  int done = 0;
  if(events & EV_READ){
    
    int r = clients_[fd]->recv_async();
    if(r <= 0 ){
      LOG(ERROR) << errno;
      clients_[fd]->disconnected();
      clients_[fd]->close();
      return done;
    }
    try{
      typedef msgpack::type::tuple<uint8_t,uint32_t,msgpack::object,msgpack::object> response_t;
      response_t res;
      clients_[fd]->salvage<response_t>(res);
      //LOG(INFO) << res.a2.type << " " <<res.a3.type << res.a2.is_nil() << " " <<res.a3.is_nil();
      ctx->rest--;
      if(res.a0 == 1){
	if(res.a2.is_nil()){
	  ctx->ret.push_back(res.a3);
	}else if(res.a3.is_nil()){
	  LOG(ERROR) << res.a2.type;
	  std::string msg;
	  res.a2.convert(&msg);
	}else{
	  LOG(ERROR) << "both a2 and a3 is not nil: "  << res.a2.type << " " <<res.a3.type;
	}
	return done;
      }
      //del(epfd_, clients_[idx]->get());
      
    }catch(const msgpack::unpack_error& e){
      //if(e.what() == "insufficient bytes"){ // gogogo...
      LOG(ERROR) << e.what() << " bytes read:" << r;
      
      //more to recieve      //mod_in(epfd_, clients_[idx]->get(), idx);
      struct timeval timeout;
      timeout.tv_sec = timeout_sec_;
      timeout.tv_usec = 0;
      event_base_once(evbase_.get(), fd, EV_READ, &mprpc::readable_callback, ctx, &timeout);
  
    }catch(const std::runtime_error& e){
      LOG(ERROR) << e.what();
    }catch(const std::exception& e){
      LOG(ERROR) << e.what();
    }
  }else if(events & EV_TIMEOUT){
    clients_[fd]->disconnected();
    clients_[fd]->close();
    ctx->rest--;
  }
  return done;
}

static void writable_callback(int fd, short int events, void* arg){
  async_context* ctx = static_cast<async_context*>(arg);
  ctx->rest -= ctx->c->writable_callback(fd, events, ctx);
}
int rpc_mclient::writable_callback(int fd, int events, async_context* ctx){
  int done = 0;
  if(events & EV_WRITE){
    if(clients_[fd]->is_connecting()){
      clients_[fd]->set_sending();
    }
    if(clients_[fd]->send_async(ctx->buf->data(), ctx->buf->size())){
      ctx->rest--;
      clients_[fd]->set_recving();
      //event_base_once(evbase_.get(), fd, EV_READ, &readable_callback, ctx);

    }else{
      struct timeval timeout;
      timeout.tv_sec = timeout_sec_;
      timeout.tv_usec = 0;
      event_base_once(evbase_.get(), fd, EV_WRITE, &mprpc::writable_callback, ctx, &timeout);
    }

  }else if(events & EV_TIMEOUT){
    clients_[fd]->disconnected();
    clients_[fd]->close();
    ctx->rest--;
  }
  return done;
}

void rpc_mclient::send_async(const msgpack::sbuffer& buf)
{
  async_context ctx;
  ctx.c = this;
  ctx.rest = clients_.size();
  ctx.buf = &buf;

  struct timeval timeout;
  timeout.tv_sec = timeout_sec_;
  timeout.tv_usec = 0;
  pfi::data::unordered_map<int,pfi::lang::shared_ptr<async_sock> >::iterator it;
  for(it=clients_.begin(); it!=clients_.end(); ++it){
    event_base_once(evbase_.get(), it->second->get(), EV_WRITE, &mprpc::writable_callback, &ctx, &timeout);
  }
  do{
    event_base_loop(evbase_.get(), EVLOOP_ONCE);
  }while(ctx.rest>0);
}


void rpc_mclient::join_some_(async_context& ctx)
{
  ctx.ret.clear();

  struct timeval timeout;
  timeout.tv_sec = timeout_sec_;
  timeout.tv_usec = 0;
  pfi::data::unordered_map<int,pfi::lang::shared_ptr<async_sock> >::iterator it;
  for(it=clients_.begin(); it!=clients_.end(); ++it){
    event_base_once(evbase_.get(), it->second->get(), EV_READ, &mprpc::readable_callback, &ctx, &timeout);
  }

  event_base_loop(evbase_.get(), EVLOOP_ONCE);
}

}
}
}

