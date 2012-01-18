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

#pragma once

#include <vector>
#include <string>

#include "../common/membership.hpp"

#include <glog/logging.h>

#include <pficommon/concurrent/thread.h>
#include <pficommon/concurrent/mutex.h>
#include <pficommon/concurrent/condition.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/function.h>

namespace jubatus{
namespace framework{

  class mixer {
  public:
    mixer(const std::string& name, unsigned int count_threshold, unsigned int tick_threshold,
          pfi::lang::function<void(const std::vector<std::pair<std::string,int> >&)> mixer_fun);
    virtual ~mixer(){};

    unsigned int updated();
    void accessed(){};
    void clear();

    void set_zk(pfi::lang::shared_ptr<jubatus::common::lock_service>& z){
      zk_ = z;
    };
    void start(){
      if(!zk_) throw std::runtime_error("zk is not initialized.");
      t_.start();
    };
    void get_status(std::map<std::string,std::string>& out);


    void mixer_loop(){
      while(true)try_mix();
    };

    int get_count()const {return counter_;} ; //FIXME: not thread-safe
    void try_mix();

  private:  

    pfi::lang::function<void(const std::vector<std::pair<std::string,int> >&)> mixer_func_;

    pfi::lang::shared_ptr<jubatus::common::lock_service> zk_;
    std::string name_;

    unsigned int count_threshold_;
    unsigned int counter_;
    unsigned int tick_threshold_;
    unsigned int ticktime_;
    unsigned int mix_count_;

    pfi::concurrent::thread t_;
    pfi::concurrent::mutex m_;
    pfi::concurrent::condition c_;
  };

}
}
