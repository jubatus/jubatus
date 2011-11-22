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

#include "mixer.hpp"

#include "./storage/local_storage.hpp"
#include "../common/membership.hpp"

#include <time.h>
#include <map>

#include <pficommon/lang/bind.h>
#include <pficommon/network/mprpc/rpc_client.h>
#include <pficommon/data/intern.h>

#include <glog/logging.h>

using namespace jubatus;
using namespace jubatus::storage;
using pfi::concurrent::scoped_lock;
using namespace pfi::data;
using pfi::lang::shared_ptr;

mixer::mixer(pfi::lang::shared_ptr<jubatus::zk>& z, const std::string& name,
             unsigned int count_threshold, unsigned int tick_threshold)
  :
  zk_(z),name_(name), 
  count_threshold_(count_threshold),
  counter_(0),
  tick_threshold_(tick_threshold),
  ticktime_(time(NULL)),
  t_(pfi::lang::bind(&mixer::mixer_loop, this))
{
}

mixer::~mixer(){}

void mixer::updated(){
  scoped_lock lk(m_);
  unsigned int new_ticktime = time(NULL);
  ++counter_;
  if(counter_ > count_threshold_ ||
     new_ticktime - ticktime_ > tick_threshold_){
    c_.notify(); // FIXME: need sync here?
  }
}
void mixer::accessed(){
}

void mixer::clear(){
  scoped_lock lk(m_);
  counter_ = 0;
  ticktime_ = time(NULL);
}

void mixer::start(){
  t_.start();
}

void mixer::try_mix(){
  jubatus::zkmutex zklock(zk_, jubatus::ACTOR_BASE_PATH +"/" + name_ + "/master_lock");
  {
    scoped_lock lk(m_);
    
    // pthread_cond_timedwait returns: ETIMEOUT=>false or 0=>true
    c_.wait(m_, 1); //FIXME
    unsigned int new_ticktime = time(NULL);
    if( counter_ > count_threshold_ || new_ticktime - ticktime_ > tick_threshold_ ){

      if(zklock.try_lock()){
        DLOG(INFO) << "mix....";
        counter_ = 0;
        ticktime_ = new_ticktime;
      }else{
        return;
      }

    }else{
      return;
    }

  } //unlock
  
  std::vector<std::pair<std::string,int> > servers;
  jubatus::get_all_actors(*zk_, name_, servers);
  mix_(servers);
  DLOG(INFO) << "....mix done";
}

void mixer::set_mixer_func(pfi::lang::function<void(const std::vector<std::pair<std::string,int> >&)> mix){
  scoped_lock lk(m_);
  mix_ = mix;
}
int mixer::get_count()const{
  return counter_;
}

void mixer::get_status(std::map<std::string,std::string>& out){
  scoped_lock lk(m_);
  out["count"] = pfi::lang::lexical_cast<std::string>(counter_);
  out["ticktime"] = pfi::lang::lexical_cast<std::string>(ticktime_); //since last mix
}

void mixer::mixer_loop(void* p){
  mixer * m = static_cast<mixer*>(p);
  while(true){
    m->try_mix();
  }
}
