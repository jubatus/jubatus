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
#include <sys/epoll.h>

using pfi::lang::shared_ptr;
using pfi::system::time::clock_time;
using pfi::system::time::get_clock_time;

namespace jubatus { namespace common { namespace mprpc {


int ep(int epfd, int fd, int ctl, int idx, int in_out){
  struct epoll_event ev;
  ev.events = in_out | EPOLLONESHOT;
  //ev.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT;
  ev.data.u64 = idx;
  int r = ::epoll_ctl(epfd, ctl, fd, &ev);
  LOG(ERROR)<<  r << " " << fd << " " << std::hex <<ev.events;
  if(r==-1){

    perror(__func__);
    return r;
  }
  return 0;
}
int del(int epfd, int fd){
  return ep(epfd, fd, EPOLL_CTL_DEL, 0, 0);
}
int mod_in(int epfd, int fd, int idx){
  return ep(epfd, fd, EPOLL_CTL_MOD, idx, EPOLLIN);
}
int mod_out(int epfd, int fd, int idx){
  return ep(epfd, fd, EPOLL_CTL_MOD, idx, EPOLLOUT);
}
int add_in(int epfd, int fd, int idx){
  return ep(epfd, fd, EPOLL_CTL_ADD, idx, EPOLLIN);
}
int add_out(int epfd, int fd, int idx){
  return ep(epfd, fd, EPOLL_CTL_ADD, idx, EPOLLOUT);
}



rpc_mclient::rpc_mclient(const std::vector<std::pair<std::string, uint16_t> >& hosts,
                         int timeout_sec):
  hosts_(hosts),
  timeout_sec_(timeout_sec),
  start_(get_clock_time()),
  epfd_(epoll_create(64))
{
  connect_async_();
}
rpc_mclient::rpc_mclient(const std::vector<std::pair<std::string, int> >& hosts,
                         int timeout_sec):
  timeout_sec_(timeout_sec),
  start_(get_clock_time()),
  epfd_(epoll_create(64))
{
  for(size_t i=0; i<hosts.size(); ++i){
    hosts_.push_back(hosts[i]);
  }
  connect_async_();
}

rpc_mclient::~rpc_mclient(){
}

void rpc_mclient::connect_async_()
{
  clients_.clear();
  for(size_t i=0; i<hosts_.size(); ++i){
    shared_ptr<async_sock> p(new async_sock);
    p->connect_async(hosts_[i].first, hosts_[i].second);
    clients_.push_back(p);
    
    add_out(epfd_, p->get(), i);
  }
}

void rpc_mclient::send_async(const msgpack::sbuffer& buf)
{
  struct epoll_event events[1024];
  size_t to_be_sent = clients_.size();
  do{
    int n = epoll_wait(epfd_, events, 1024, timeout_sec_);
    if(n<0){
      LOG(ERROR) << n;
      return;
    }
    for(int i=0; i<n; ++i){
      size_t idx = events[i].data.u64;
      if(events[i].events & EPOLLOUT){
        if(clients_[idx]->is_connecting()){
          clients_[idx]->set_sending();
        }

        if(clients_[idx]->send_async(buf.data(), buf.size())){
          //DLOG(INFO) << buf.size() << " bytes sent to " << hosts_[idx].second;
          to_be_sent--;
          clients_[idx]->set_recving();
          mod_in(epfd_, clients_[idx]->get(), idx);

        }else{
          mod_out(epfd_, clients_[idx]->get(), idx);
        }
      }
      if(events[i].events & (EPOLLERR | EPOLLHUP)){
        to_be_sent--;
        clients_[idx]->disconnected();
        clients_[idx]->close();
      }
    }
  }while(to_be_sent>0);
}


int rpc_mclient::join_some_(std::vector<msgpack::object>& ret, int rest)
{
  ret.clear();

  int done = 0;
  struct epoll_event events[1024];
  int n = epoll_wait(epfd_, events, 1024, timeout_sec_);

  if(n<0){
    LOG(ERROR) << n;
    return rest; // throw something
  }
  for(int i=0; i<n; ++i){
    size_t idx = events[i].data.u64;

    if(events[i].events & EPOLLIN){

      if(not clients_[idx]->is_recving()){
        clients_[idx]->disconnected();
        done++;

        clients_[idx]->close();
        continue;
      }
      
      int r = clients_[idx]->recv_async();
      if(r < 0 ){
        LOG(ERROR) << errno;
        return done;
      }else if(r==0){
        LOG(INFO) << "r==0 errno:" << errno;
        continue;
      }
      try{
        typedef msgpack::type::tuple<uint8_t,uint32_t,msgpack::object,msgpack::object> response_t;
	response_t res;
	clients_[idx]->salvage<response_t>(res);
	//LOG(INFO) << res.a2.type << " " <<res.a3.type << res.a2.is_nil() << " " <<res.a3.is_nil();
        done++;
        if(res.a0 == 1){
	  if(res.a2.is_nil()){
	    ret.push_back(res.a3);
	  }else if(res.a3.is_nil()){
	    LOG(ERROR) << res.a2.type;
	    std::string msg;
	    res.a2.convert(&msg);
	    throw std::runtime_error(msg);
	  }else{
	    LOG(ERROR) << "both a2 and a3 is not nil: "  << res.a2.type << " " <<res.a3.type;
	    done++;
	    continue;
	  }
	}
	del(epfd_, clients_[idx]->get());

      }catch(const msgpack::unpack_error& e){
      //if(e.what() == "insufficient bytes"){ // gogogo...
        LOG(ERROR) << e.what() << " bytes read:" << r;

        //more to recieve
        mod_in(epfd_, clients_[idx]->get(), idx);
        continue;
      //throw e;
      }catch(const std::runtime_error& e){
        LOG(ERROR) << e.what();
        throw e;
      }catch(const std::exception& e){
	LOG(ERROR) << e.what();
        throw e;
      }
    }

    if(events[i].events & (EPOLLERR | EPOLLHUP)){
      done++;
      clients_[idx]->disconnected();
      clients_[idx]->close();
    }
  }
  return rest - done;
}

}
}
}

