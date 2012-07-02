// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include "exception.hpp"
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

  cht::cht(cshared_ptr<lock_service> z, const std::string& name):
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
  
  bool cht::find(const std::string& host, int port, std::vector<std::pair<std::string,int> >& out, size_t s){
    return find(build_loc_str(host, port), out, s);
  }

  // return at most n nodes, if theres nodes less than n, return size is also less than n.
  // find(hash)    :: lock_service -> key -> [node] where hash(node0) <= hash(key) < hash(node1)
  bool cht::find(const std::string& key, std::vector<std::pair<std::string,int> >& out, size_t n){
    out.clear();
    std::vector<std::string> hlist;
    if(! get_hashlist_(key, hlist)){
      throw JUBATUS_EXCEPTION(not_found(key));
    }
    std::string hash = make_hash(key);
    std::string path = ACTOR_BASE_PATH + "/" + name_ + "/cht";

    std::vector<std::string>::iterator node0 = std::lower_bound(hlist.begin(), hlist.end(), hash);
    size_t idx = int(node0 - hlist.begin()) % hlist.size();
    std::string loc;
    for(size_t i=0; i<n; ++i){
      std::string ip;
      int port;
      if(lock_service_->read(path + "/" + hlist[idx], loc)){
        revert(loc, ip, port);
        out.push_back(make_pair(ip,port));
      }else{
        // TODO: output log
        throw JUBATUS_EXCEPTION(not_found(path));
      }
      idx++;
      idx %= hlist.size();
    }
    return !hlist.size();
  }

  std::pair<std::string,int> cht::find_predecessor(const std::string& host, int port){
    return find_predecessor(build_loc_str(host, port));
  }
  std::pair<std::string,int> cht::find_predecessor(const std::string& key){
    std::vector<std::string> hlist;
    get_hashlist_(key, hlist);

    std::string hash = make_hash(key);
    std::string path = ACTOR_BASE_PATH + "/" + name_ + "/cht";

    std::vector<std::string>::iterator node0 = std::lower_bound(hlist.begin(), hlist.end(), hash);
    size_t idx = (int(node0 - hlist.begin())+ hlist.size() -1) % hlist.size();

    std::string ip;
    int port;
    std::string loc;
    if(lock_service_->read(path + "/" + hlist[idx], loc)){
      revert(loc, ip, port);
      return make_pair(ip, port);
    }else{
      throw JUBATUS_EXCEPTION(not_found(path));
      // TODO: output log and throw exception
    }
  }

  void cht::setup_cht_dir(lock_service& ls, const std::string& name){
    std::string path = ACTOR_BASE_PATH + "/" + name;
    ls.create(path, "");
    path +=  "/cht";
    ls.create(path, "");
  }

  bool cht::get_hashlist_(const std::string& key, std::vector<std::string>& hlist){
    hlist.clear();
    std::string path = ACTOR_BASE_PATH + "/" + name_ + "/cht";
    std::vector<std::pair<std::string, int> > ret;
    lock_service_->list(path, hlist);

    if(hlist.empty()) return false;
    std::sort(hlist.begin(), hlist.end());
    return true;
  }

}
}
