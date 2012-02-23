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

namespace jubatus { namespace common { namespace mprpc {

rpc_mclient::rpc_mclient(const std::vector<std::pair<std::string, uint16_t> >& hosts,
                         int timeout_sec):
  hosts_(hosts),
  timeout_sec_(timeout_sec)
{
}

rpc_mclient::~rpc_mclient(){
}

void rpc_mclient::send_async(const std::string& method, const msgpack::sbuffer& buf)
{
  // FIXME:
  //  currently cannot ignore faulty client
  //  when one faulty client exceeds timeout, RPCs to other clients all fail.

  for(size_t i=0; i<hosts_.size(); ++i){
    clients_.push_back(async_client(hosts_[i].first, hosts_[i].second, timeout_sec_));
  }
  for(size_t i=0; i<clients_.size(); ++i){
    clients_[i].send_async(method, buf);
  }
}

      /*
dclient::dclient(const std::vector<std::pair<std::string, uint16_t> >& clients,
                 int timeout_sec):
  clients_(clients),
  timeout_sec_(timeout_sec)
{
  if(clients_.empty())
    throw std::runtime_error("empty multi-rpc");
}
dclient::~dclient(){
}

void dclient::send_async(const std::string& method, const msgpack::object& argv){

  std::vector<shared_ptr<async_sock> > sockets;
  int epfd = epoll_create(1024);

  for(size_t i=0; i<clients_.size(); ++i){
    shared_ptr<async_sock> s(new async_sock);
    int res = s->connect_async(clients_[i].first, clients_[i].second); // TODO: error check
    sockets.push_back(s);
    res;
    struct epoll_event ev;
    ev.events = EPOLLOUT | EPOLLONESHOT;
    ev.data.ptr = s.get();
    res = epoll_ctl(epfd, EPOLL_CTL_ADD, s->get(), &ev); // TODO: error check
  }
  
  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, argv);
  size_t count = sockets.size();
  while(count>0){
    const size_t N = 64;
    struct epoll_event evs[N];
    int n = epoll_wait(epfd, evs, N, 200);
    for(int i=0; i<n; ++i){
      async_sock* p = reinterpret_cast<async_sock*>(evs[i].data.ptr);
      if(evs[i].events == EPOLLOUT){
        if(p->is_connecting()){
          p->set_sending();
        }
        if(p->is_sending()){
          if(p->send_async(sbuf.data(), sbuf.size())){
            p->set_recving();
            evs[i].events = EPOLLIN | EPOLLONESHOT;
            epoll_ctl(epfd, EPOLL_CTL_MOD, p->get(), &evs[i]);
          }else{
            evs[i].events = EPOLLOUT | EPOLLONESHOT;
            epoll_ctl(epfd, EPOLL_CTL_MOD, p->get(), &evs[i]);
          }

        }else if(p->is_recving()){
          //          if(p->recv_async(
        }
      }
      else if(evs[i].events == EPOLLERR){
        p->close();
      }
    }
  }
}
      */

}
}
}

