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

#include "../exception.hpp"

#include <fcntl.h>
#include <string.h>
#include <errno.h>

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
  //pfi::network::mprpc::socket(::socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0)),
  pfi::network::mprpc::socket(::socket(AF_INET, SOCK_STREAM, 0)),
  state(CLOSED),
  progress(0)
{
  // FIXME: SOCK_NONBLOCK is linux only
  int fd = get();
  set_socket_nonblock(fd, true);
  unpacker.reserve_buffer(4096);
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
  if(unpacker.message_size() == 0){
    unpacker.reset();
    unpacker.reserve_buffer(4096);
  }else if(unpacker.buffer_capacity() == 0){
    //unpacker.expand_buffer(4096);
  }
  int r = ::read(fd, unpacker.buffer(), unpacker.buffer_capacity());
  // if(r < 0){
  //   char msg[1024];
  //   strerror_r(errno, msg, 1024);
  //   cout << "errno:"<< errno <<  msg << endl;
  // }
  if(r > 0){
    unpacker.buffer_consumed(r);
  }
  return r;
};

int async_sock::connect_async(const std::string& host, uint16_t port){
  int res;
  int sock = this->get();
  
  std::vector<pfi::network::ipv4_address> ips = resolve(host, port);
  for (int i=0; i < (int)ips.size(); i++){
    sockaddr_in addrin={};
    addrin.sin_family = PF_INET;
    addrin.sin_addr.s_addr = inet_addr(ips[i].to_string().c_str());
    addrin.sin_port = htons(port);
    
    res = ::connect(sock,(sockaddr*)&addrin,sizeof(addrin));
    if (res == -1){
      if (errno==EINPROGRESS){
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


}
}
}
