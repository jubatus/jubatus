// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "cht.hpp"
#include "membership.hpp"
#include <stdlib.h>

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <unistd.h>

namespace jubatus{

  // this function does not seem pure, take care when calling from multiple threads
  std::string make_hash(const std::string& key){
    char * h = crypt(key.c_str(), SALT_BASE.c_str());
    std::string ret(h+SALT_BASE.size());
    for(size_t s = 0; s < ret.size(); ++s){
      if(ret[s] == '/') ret[s]='p';
    }
    //free(h); don't know why this makes double free...
    //std::cout << key << " => " << ret << std::endl;
    return ret;
  };


  cht::cht(zk& z, const std::string& name):
    name_(name), zk_(&z)
  {
    std::string path = ACTOR_BASE_PATH + "/" + name_;
    zk_->create(path, "");
    path +=  "/cht";
    zk_->create(path, "");
  }

  cht::~cht(){}

  // register_node :: node -> bool;
  // creates /jubatus/actors/cht/<hash(ip_port)> with contents ip_port
  bool cht::register_node(const std::string& ip, int port){
    std::string path = ACTOR_BASE_PATH + "/" + name_ + "/cht";

    for(unsigned int i=0; i<NUM_VSERV; ++i){
      std::string hashpath = path+"/"+make_hash(build_loc_str(ip, port, i));
      zk_->create(hashpath, build_loc_str(ip,port), true);
      DLOG(INFO) << "created " << hashpath;
    }
    //TODO: watch your predecessor!
    // if you find your predecessor dead, replicate all data you have to your successor
    return true;
  }
  
  bool cht::find(const std::string& host, int port, std::vector<std::pair<std::string,int> >& out){
    return find(build_loc_str(host, port), out);
  }

  // find(hash)    :: zk -> key -> [node] where hash(node0) <= hash(key) < hash(node1)
  bool cht::find(const std::string& key, std::vector<std::pair<std::string,int> >& out){
    out.clear();
    std::string path = ACTOR_BASE_PATH + "/" + name_ + "/cht";
    std::string hash = make_hash(key);
    std::vector<std::pair<std::string, int> > ret;
    std::vector<std::string> hlist;
    zk_->list(path, hlist);

    if(hlist.empty()) return false;
    std::sort(hlist.begin(), hlist.end());

    std::vector<std::string>::iterator node0 = std::lower_bound(hlist.begin(), hlist.end(), hash);
    size_t idx = int(node0 - hlist.begin()) % hlist.size();
    std::string loc;
    for(int i=0; i<2; ++i){
      std::string ip;
      int port;
      if(zk_->read(path + "/" + hlist[idx], loc)){
        revert(loc, ip, port);
        out.push_back(make_pair(ip,port));
      }else{
        // TODO: log output
      }
      idx++;
      idx %= hlist.size();
    }
    return !hlist.size();
  }

}

