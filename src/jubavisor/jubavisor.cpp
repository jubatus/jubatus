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

#include <pficommon/concurrent/lock.h>

#include <glog/logging.h>

#include "jubavisor.hpp"
#include "../common/util.hpp"
#include "../common/membership.hpp"

using namespace jubatus;
using namespace jubatus::common;
using namespace pfi::lang;
using pfi::concurrent::scoped_lock;

jubervisor::jubervisor(const std::string& hosts, int port, int max,
                       const std::string& logfile):
  zk_(create_lock_service("zk", hosts, 1024, logfile)),
  port_base_(port),
  logfile_(logfile),
  max_children_(max)
{
  zk_->create(JUBATUS_BASE_PATH, "");
  zk_->create(JUBAVISOR_BASE_PATH, "");
  zk_->create(ACTOR_BASE_PATH, "");

  name_ = build_loc_str(util::get_ip("eth0"), port);
  std::string path = JUBAVISOR_BASE_PATH + "/" + name_;
  zk_->create(path, "", true);
  // FIXME: 
  //  pfi::lang::function<void(int,int,std::string)> f = bind(&jubervisor::die_if_deleted, this, _1, _2, _3);
  //  zk_->bind_watcher(path, f);
  DLOG(INFO) << path << " created.";

  for(unsigned int i=0;i<max_children_;++i){
    port_pool_.push(++port_base_);
  }

  pfi::lang::function<void()> h = bind(&jubervisor::stop_all, this);
  zk_->push_cleanup(h);
  pfi::lang::function<void()> g = bind(&exit, -1);
  zk_->push_cleanup(g);
}
jubervisor::~jubervisor(){
  stop_all();
}


// str : "<type>/<name>[/<storage>]"
// type : "classifier" ...
// name : any but ""
// -> exec ./jubatus_<type> -n <name> -p <rpc_port> -z <zk>
int jubervisor::start(std::string str, unsigned int N){
  scoped_lock lk(m_);
  LOG(INFO) << str << " " << N;
  return start_(str, N);
}

int jubervisor::start_(const std::string& str, unsigned int N){
  
  std::string name;
  {
    process p(zk_->get_hosts());
    if(!p.set_names(str)){
      LOG(ERROR) << "cannot parse " << str;
      return -1;
    }
    name = p.get_name();
  }
  std::map<std::string, std::vector<process> >::iterator it = children_.find(name);
  if(it == children_.end()){
    children_.insert(make_pair(name, std::vector<process>()));    
  }
  it = children_.find(name);
  if(it->second.size() > N){
    LOG(ERROR) << it->second.size() << " " << name << " already running at this machine.";
    return -1;
  }
  N -= it->second.size();

  if(port_pool_.size() < N){
    LOG(ERROR) << "cannot spawn more than " << max_children_ << " processes.";
    LOG(ERROR) << "currently remaining " << port_pool_.size() << " while " << N << " requested.";
    return -1;
  }
  
  for(unsigned int n=0; n<N; ++n){
    process p(zk_->get_hosts());
    p.set_names(str);
    it = children_.find(name);

    int p1 = port_pool_.front(); port_pool_.pop();
    if(p.spawn_link(p1)){
      it->second.push_back(p);
      
    }else{
      LOG(ERROR) << "cannot start: " << p.get_name();
      port_pool_.push(p1);
      return -1;
    }
  }
  return 0;
}

int jubervisor::stop(std::string str, unsigned int N){
  DLOG(ERROR) << str;
  process p(zk_->get_hosts());
  p.set_names(str);
  std::vector<process> v;
  {
    scoped_lock lk(m_);
    std::map<std::string, std::vector<process> >::iterator it = children_.find(p.get_name());
    if(it==children_.end()){
      LOG(INFO) << p.get_name() << " is not in children.";
      return 0;
    }
    v = it->second;
    children_.erase(it);
  }
  int r = 0;

  for(size_t i = 0; i < v.size(); ++i){
    if(!v[i].kill()){
      r--;
    }else{
      port_pool_.push(v[i].get_rpc_port());
    }
  }
  return r;
}

void jubervisor::stop_all(){
  DLOG(ERROR) << __func__;
  scoped_lock lk(m_);
  std::map<std::string,std::vector<process> >::iterator it;
  for(it = children_.begin(); it!=children_.end(); ++it){
    for(size_t i = 0; i < it->second.size(); ++i){
      if(!it->second[i].kill()){
        LOG(ERROR) << "failed stopping process: name(" << it->second[i].get_name()
                   << ") type(" << it->second[i].get_type()
                   << ") storage(" << it->second[i].get_storage()
                   << ") ";
        perror(__func__);
      }
    }
  }
  children_.clear();
}

// void jubervisor::die_if_deleted(int type, int state, std::string path){
//   if(type == ZOO_DELETED_EVENT){
//     LOG(INFO) << "terminating: deleted " << path << " at lock service.";
//     this->stop_all();
//     exit(-1);
//   }
// }

  /*

// jubatusctl ensure type/name[/storage]
int jubervisor::ensure(std::string str){
  process p(zk_->get_hosts());
  p.set_names(str);

  scoped_lock lk(m_);
  std::map<std::string,process>::const_iterator it = children_.find(str);
  if(it!=children_.end()){

    if( p.has_samespec(it->second) ){
      return 0;
    }

    // wrong spec, thus restart with new spec
    int r = stop_(str, p);
    if(!(r == 0 && p.kill())){ // FIXME: kill takes rather long time but can't release lock.
      LOG(ERROR) << "failed stopping process";
      return r;
    }
  }
  return start_(str);
}
  */
