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
#include <glog/logging.h>

#include <pficommon/data/digest/md5.h>
#include <sstream>

namespace jubatus{
namespace common{

  std::string make_hash(const std::string& key){
    std::stringstream ss;
    ss << pfi::data::digest::md5sum(key);
    return ss.str();
  };

  cht::cht(pfi::lang::shared_ptr<lock_service> z, const std::string& name):
    name_(name), lock_service_(z)
  {
  }

  cht::~cht(){}

  // register_node :: node -> bool;
  // creates /jubatus/actors/<name>/cht/<hash(ip_port)> with contents ip_port
  void cht::register_node(const std::string& ip, int port){
    std::string path = ACTOR_BASE_PATH + "/" + name_ + "/cht";

    for(unsigned int i=0; i<NUM_VSERV; ++i){
      std::string hashpath = path+"/"+make_hash(build_loc_str(ip, port, i));
      lock_service_->create(hashpath, build_loc_str(ip,port), true);
      DLOG(INFO) << "created " << hashpath;
    }
  }
  
  bool cht::find(const std::string& host, int port, std::vector<std::pair<std::string,int> >& out){
    return find(build_loc_str(host, port), out);
  }

  // find(hash)    :: lock_service -> key -> [node] where hash(node0) <= hash(key) < hash(node1)
  bool cht::find(const std::string& key, std::vector<std::pair<std::string,int> >& out){
    out.clear();
    std::string path = ACTOR_BASE_PATH + "/" + name_ + "/cht";
    std::string hash = make_hash(key);
    std::vector<std::pair<std::string, int> > ret;
    std::vector<std::string> hlist;
    lock_service_->list(path, hlist);

    if(hlist.empty()) return false;
    std::sort(hlist.begin(), hlist.end());

    std::vector<std::string>::iterator node0 = std::lower_bound(hlist.begin(), hlist.end(), hash);
    size_t idx = int(node0 - hlist.begin()) % hlist.size();
    std::string loc;
    for(int i=0; i<2; ++i){
      std::string ip;
      int port;
      if(lock_service_->read(path + "/" + hlist[idx], loc)){
        revert(loc, ip, port);
        out.push_back(make_pair(ip,port));
      }else{
        // TODO: output log
      }
      idx++;
      idx %= hlist.size();
    }
    return !hlist.size();
  }

  void setup_cht_dir(lock_service& ls, const std::string name){
    std::string path = ACTOR_BASE_PATH + "/" + name;
    ls.create(path, "");
    path +=  "/cht";
    ls.create(path, "");
  }

}
}
