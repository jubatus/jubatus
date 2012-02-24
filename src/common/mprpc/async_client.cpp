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

#include "async_client.hpp"
#include <pficommon/network/mprpc/socket.h>
#include <pficommon/network/mprpc/exception.h>
#include <pficommon/network/ipv4.h>

#include "../exception.hpp"

#include <fcntl.h>

// fixme: use picoev
#include <sys/epoll.h>

using pfi::lang::shared_ptr;
using pfi::network::mprpc::socket;
using pfi::system::time::clock_time;
using pfi::system::time::get_clock_time;

namespace jubatus { namespace common { namespace mprpc {

bool set_socket_nonblock(int sock, bool on){
  int res;
  if(on){
    res = fcntl(sock, F_SETFL, O_NONBLOCK);
    return res==0;
  }else{
    //FIXME
    return false;
  }
}

class async_sock : public pfi::network::mprpc::socket {
public:
  async_sock():
    pfi::network::mprpc::socket(::socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0)),
    state(CLOSED),
    progress(0)
  {
    // FIXME: SOCK_NONBLOCK is linux only
    int fd = get();
    set_socket_nonblock(fd, true);
  }
  ~async_sock(){}
  bool set_async(bool on){ return on; }
  bool send_async(const char* buf, size_t size){
    int fd = this->get();
    int r = ::write(fd, buf+progress, size-progress);
    if(r > 0){
      progress += r;
    }
    if(progress == size){
      progress = 0;
      return true;
    }
    return size == progress;
  };
  void recv_async(char* buf, size_t size){
    int fd = this->get();
    int r = ::read(fd, buf+progress, size-progress);
    if(r > 0){
      progress += r;
    }
    if(progress == size){
      progress = 0;
    }
    return;
  };
  //msgpack::object get_retval(){ return msgpack::type::nil(); }; 
  
  int connect_async(const std::string& host, uint16_t port){
    int res;
    int sock = get();
    
    std::vector<pfi::network::ipv4_address> ips = resolve(host, port);
    for (int i=0; i < (int)ips.size(); i++){
      sockaddr_in addrin={};
      addrin.sin_family = PF_INET;
      addrin.sin_addr.s_addr = inet_addr(ips[i].to_string().c_str());
      addrin.sin_port = htons(port);
      
      res = ::connect(sock,(sockaddr*)&addrin,sizeof(addrin));
      if (res==0){
        state = CONNECTING;
        return 0;
      }
    }
    ::close(sock);
    return -1;
  }

  // bool process(){
  //   switch(state){
  //   case CLOSED: return false;
  //   case CONNECTING:
         
  //     break;
  //   case CONNECTED:
  //     break;
  //   default:
  //     return false;
  //   }
  // };

  void close(){ ::close(this->get()); disconnected(); };
  void set_sending(){ state = SENDING; };
  void set_recving(){ state = RECVING; };
  void disconnected(){ state = CLOSED; };
  bool is_connecting()const{ return state == CONNECTING; };
  bool is_sending()const{ return state == SENDING; };
  bool is_recving()const{ return state == RECVING; };
private:
  enum { CLOSED, CONNECTING, SENDING, RECVING } state;
  size_t progress;
};


async_client::async_client(const std::string& host, uint16_t port, int timeout_sec):
  host_(host),
  port_(port),
  timeout_sec_(timeout_sec),
  start_(get_clock_time())
{
  connect_async();
}

async_client::~async_client(){
  if(sock_)sock_->close();
}

void async_client::connect_async(){
  sock_ = shared_ptr<async_sock>(new async_sock);
  int res = sock_->connect_async(host_, port_);
  if(res != 0){
    // TODO: error handling
    return;
  }
}

bool async_client::wait(){
  // messssssy
  int epfd = ::epoll_create(1);
  struct epoll_event ev;
  ev.events = EPOLLOUT | EPOLLIN | EPOLLONESHOT;
  ev.data.fd = sock_->get();
  int res = epoll_ctl(epfd, EPOLL_CTL_ADD, ev.data.fd, &ev); // TODO: error check
  if(res != 0){
    // TODO: error handling
    ::close(epfd);
    return false;
  }
  res = ::epoll_wait(epfd, &ev, 1, timeout_sec_);
  ::close(epfd);
  return (ev.events & EPOLLOUT) || (ev.events & EPOLLIN);
}

void async_client::send_async(const std::string& method, const msgpack::sbuffer& sbuf){

  start_ = get_clock_time();
  if(!wait()){
    return; // TODO: erro handling
  }
  
  do{
    if(sock_->send_async(sbuf.data(), sbuf.size())){
      return;
    }
    wait();
    if((double)(get_clock_time() - start_) > timeout_sec_ ){
      throw pfi::network::mprpc::rpc_timeout_error("");
    }
  }while(true);

}

void async_client::join(msgpack::object& o){

  if(!wait()){
    //TODO: error handling
  }

  msgpack::sbuffer sbuf;

  do{
    sock_->recv_async(sbuf.data(), sbuf.size());

    try{ // a bit slow but simple
      msgpack::unpacked msg;
      msgpack::unpack(&msg, sbuf.data(), sbuf.size());
      o = msg.get();
      return;
    }catch(const msgpack::unpack_error& e){
      if(e.what() == "insufficient bytes"){ // gogogo...
        continue;
      }
      throw e;
    }catch(const std::exception& e){
      throw e;
    }

    wait();  
    if((double)(get_clock_time() - start_) > timeout_sec_ ){
      throw pfi::network::mprpc::rpc_timeout_error("");
    }
  }while(true);
}

}
}
}
