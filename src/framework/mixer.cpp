// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <pficommon/lang/bind.h>
#include "../common/exception.hpp"

namespace jubatus {
namespace framework {

  mixer::mixer(const std::string& type, const std::string& name,
               unsigned int count_threshold, unsigned int tick_threshold,
               pfi::lang::function<void(const std::vector<std::pair<std::string,int> >&)> mixer_fun)
    :mixer_func_(mixer_fun),
     type_(type),
     name_(name), 
     count_threshold_(count_threshold),
     counter_(0),
     tick_threshold_(tick_threshold),
     ticktime_(time(NULL)),
     mix_count_(0),
     t_(pfi::lang::bind(&jubatus::framework::mixer::mixer_loop, this))
  {
  }

  unsigned int mixer::updated(){
    pfi::concurrent::scoped_lock lk(m_);
    unsigned int new_ticktime = time(NULL);
    ++counter_;
    if(counter_ > count_threshold_ ||
       new_ticktime - ticktime_ > tick_threshold_){
      c_.notify(); // FIXME: need sync here?
    }
    return counter_;
  }

  void mixer::clear(){
    pfi::concurrent::scoped_lock lk(m_);
    counter_=0;
    ticktime_ = time(NULL);
  }

  void mixer::get_status(std::map<std::string,std::string>& out){
    pfi::concurrent::scoped_lock lk(m_);
    out["count"] = pfi::lang::lexical_cast<std::string>(counter_);
    out["ticktime"] = pfi::lang::lexical_cast<std::string>(ticktime_); //since last mix
  }


  void mixer::try_mix(){
    std::string path;
    common::build_actor_path(path, type_, name_);
    jubatus::common::lock_service_mutex zklock(*zk_, path + "/master_lock");
    try {
      {
        pfi::concurrent::scoped_lock lk(m_);

        c_.wait(m_, 1);
        unsigned int new_ticktime = time(NULL);
        if( counter_ > count_threshold_ || new_ticktime - ticktime_ > tick_threshold_ ){
          if(zklock.try_lock()){
            DLOG(INFO) << "starting mix:";
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
      common::get_all_actors(*zk_, type_, name_, servers);
      mixer_func_(servers);
      mix_count_++;
      DLOG(INFO) << ".... " << mix_count_ << "th mix done.";

    } catch (const jubatus::exception::jubatus_exception& e) {
      LOG(ERROR) << e.diagnostic_information(true);
    }
  }

} // framework
} // jubatus
