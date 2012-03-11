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

#include <iostream>

#include <glog/logging.h>

#include "../exception.hpp"

#include <fcntl.h>
#include <string.h>
#include <errno.h>

// fixme: use picoev
#include <sys/epoll.h>
#include <sys/ioctl.h>

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

async_sock::async_sock():
  pfi::network::mprpc::socket(::socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0)),
  //pfi::network::mprpc::socket(::socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC, 0)),
  state(CLOSED),
  progress(0)
{
  // FIXME: SOCK_NONBLOCK is linux only
  int fd = get();
  set_socket_nonblock(fd, true);
}
      
async_sock::~async_sock(){
}

bool async_sock::set_async(bool on){ return on; }

bool async_sock::send_async(const char* buf, size_t size){
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

int async_sock::recv_async()
{
  int fd = this->get();

  /**
  int nread = -1;
  socklen_t len = sizeof(nread);
  ioctl(fd, FIONREAD, &nread);
  LOG(ERROR) << nread << " bytes to read";
  if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &nread, &len) != 0){
    LOG(ERROR) << "broken: " << nread;
  }else{
    LOG(ERROR) << "not broken: " << nread;
  }
  **/

  char buf[4096];
  int r = ::read(fd, buf, 4096);
  recv_buf.write(buf, r);

  if(r < 0){
    // char msg[1024];
    // strerror_r(errno, msg, 1024);
    return r;
  }
  if(r > 0){
    progress += r;
  }

  return progress;
};

msgpack::object async_sock::get_obj()const
{
  msgpack::unpacked msg;
  msgpack::unpack(&msg, recv_buf.data(), recv_buf.size());
  return msg.get();
}

int async_sock::connect_async(const std::string& host, uint16_t port){
  int res;
  int sock = this->get();
  //LOG(ERROR) << "sock_->get() " << sock;
  
  std::vector<pfi::network::ipv4_address> ips = resolve(host, port);
  for (int i=0; i < (int)ips.size(); i++){
    sockaddr_in addrin={};
    addrin.sin_family = PF_INET;
    addrin.sin_addr.s_addr = inet_addr(ips[i].to_string().c_str());
    addrin.sin_port = htons(port);
    
    //DLOG(INFO) << "connecting to " << host << ":" << port << " fd:" << sock;
    res = ::connect(sock,(sockaddr*)&addrin,sizeof(addrin));
    if (res == -1){
      if (errno==EINPROGRESS){
        //DLOG(INFO) << "EINPROGRESS";
        state = CONNECTING;
        return 0;
      }else{
        DLOG(ERROR) << errno;
      }
    }
    else if(res == 0){
      state = SENDING;
      return 0;
    }
  }
  ::close(sock);
  return -1;
}

int async_sock::close(){
  return ::close(this->get());
}

//async_sock::
  // void set_sending(){ state = SENDING; };
  // void set_recving(){ state = RECVING; };
  // void disconnected(){ state = CLOSED; };
  // bool is_connecting()const{ return state == CONNECTING; };
  // bool is_sending()const{ return state == SENDING; };
  // bool is_recving()const{ return state == RECVING; };

async_client::async_client(const std::string& host, uint16_t port, int timeout_sec):
  host_(host),
  port_(port),
  timeout_sec_(timeout_sec),
  start_(get_clock_time()),
  epfd_(::epoll_create(64))
{
  connect_async();
}

async_client::~async_client(){
  if(sock_)sock_->close();
}

void async_client::connect_async(){
  sock_ = shared_ptr<async_sock>(new async_sock);
  //LOG(ERROR) << "sock_->get() " << sock_->get(); 
  int res = sock_->connect_async(host_, port_);
  if(res != 0){
    // TODO: error handling
  }
}

void async_client::l(){
    int r;
    int e = -1;
    socklen_t len = sizeof(int);
    r = getsockopt(sock_->get(), SOL_SOCKET, SO_ERROR, &e, &len);
    if(r!=0)perror("");
}

bool async_client::wait(){
  // messssssy

  struct epoll_event ev;
  ev.events = EPOLLOUT | EPOLLIN | EPOLLONESHOT;
  ev.data.fd = sock_->get();
  int res = epoll_ctl(epfd_, EPOLL_CTL_ADD, ev.data.fd, &ev); // TODO: error check
  if(res != 0){
    // TODO: error handling
    //LOG(ERROR) << res << " " << ev.data.fd << " " << epfd_ << " " << sock_->get();
    
    int r;
    int e = -1;
    socklen_t len = sizeof(int);
    r = getsockopt(ev.data.fd, SOL_SOCKET, SO_ERROR, &e, &len);
    if(r!=0)perror("");
    else //LOG(ERROR) << e << "  " << len << " " << r;

    perror("<<<<<<<<<");
    //    ::close(epfd_);
    return false;
  }
  res = ::epoll_wait(epfd_, &ev, 1, timeout_sec_);

  //DLOG(INFO) << (ev.events & EPOLLOUT) << " connected?" << (ev.events & EPOLLIN);
  return (ev.events & EPOLLOUT) || (ev.events & EPOLLIN);
}

void async_client::send_async(const std::string& method, const msgpack::sbuffer& sbuf){

  start_ = get_clock_time();
  //LOG(ERROR) << sock_->get();
  if(!wait()){
    //LOG(ERROR) << __func__;
    perror("");
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
    //LOG(ERROR) << __func__;
    perror("");
    //TODO: error handling
  }

  msgpack::sbuffer sbuf;

  do{
    int r = sock_->recv_async();
    if(r < 0){
      //throw r; TODO
      //LOG(ERROR) << errno;
      return;
    }else if(r==0){
      continue;
    }

    try{ // a bit slow but simple
      o = sock_->get_obj();
      return;
    }catch(const msgpack::unpack_error& e){
      //if(e.what() == "insufficient bytes"){ // gogogo...
      //LOG(ERROR) << e.what();
      continue;
      //throw e;
    }catch(const std::runtime_error& e){
      //LOG(ERROR) << e.what();
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
