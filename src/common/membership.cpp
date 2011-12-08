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

#include "membership.hpp"

#include <cstdlib>

#include <pficommon/lang/cast.h>
using namespace pfi::lang;

namespace jubatus{
namespace common{

  // "127.0.0.1" -> 9199 -> "127.0.0.1_9199"
  std::string build_loc_str(const std::string& ip, int port, unsigned int i){
    std::string ret = ip + "_" + lexical_cast<std::string,int>(port);;
    if(i>0){
      ret += "_";
      ret += lexical_cast<std::string,int>(i);
    }
    return ret;
  }

  // /path/base -> 127.0.0.1 -> 9199 -> /path/base/127.0.0.1_9199
  bool build_existence_path(const std::string& base, const std::string& ip, int port, std::string& out){
    out = base + "/" + ip + "_" + lexical_cast<std::string,int>(port);
    return true;
  }

  // 127.0.0.1_9199 -> (127.0.0.1, 9199)
  bool revert(const std::string& name, std::string& ip, int& port){
    ip = name.substr(0, name.find("_"));
    port = atoi(name.substr(name.find("_") + 1).c_str());
    return true;
  }

  // zk -> name -> ip -> port -> bool
  bool register_actor(lock_service& z, const std::string& name, const std::string& ip, int port){

    std::string path = ACTOR_BASE_PATH + "/" + name;
    z.create(path, "");
    z.create(path + "/master_lock", "");
    path += "/nodes";
    z.create(path , "");
    {
      std::string path1;
      build_existence_path(path, ip, port, path1); 
      z.create(path1, "", true);
    }

    // set exit zlistener here
    pfi::lang::function <void()> f = &force_exit;
    z.push_cleanup(f);

    return true;
  }

  bool register_keeper(lock_service& z, const std::string& ip, int port){
    std::string path = JUBAKEEPER_BASE_PATH;
    z.create(path, "");
    {
      std::string path1;
      build_existence_path(path, ip, port, path1); 
      z.create(path1, "", true);
    }
    // set exit zlistener here
    pfi::lang::function <void()> f = &force_exit;
    z.push_cleanup(f);
    return true;
  }

  // zk -> name -> list( (ip, rpc_port) )
  bool get_all_actors(lock_service& z, const std::string& name, std::vector<std::pair<std::string, int> >& ret){
    ret.clear();
    std::string path = ACTOR_BASE_PATH + "/" + name + "/nodes";
    std::vector<std::string> list;
    z.list(path, list);
    for(std::vector<std::string>::const_iterator it = list.begin();
        it != list.end(); ++it ){
      std::string ip;
      int port;
      revert(*it, ip, port);
      ret.push_back(make_pair(ip,port));
    }
    return true;
  }

  bool push_cleanup(lock_service& z, pfi::lang::function<void()>& f){
    z.push_cleanup(f);
    return true;
  }

  void force_exit(){
    exit(-1);
  }
}
}
