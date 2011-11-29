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
#include "../storage/storage_base.hpp"

#include "../common/membership.hpp"

#include <pficommon/concurrent/thread.h>
#include <pficommon/concurrent/mutex.h>
#include <pficommon/concurrent/condition.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/function.h>

namespace jubatus{

  class mixer{
  public:
    void updated();
    void accessed();
    void clear();
    void start();
    void get_status(std::map<std::string,std::string>&);

    mixer(pfi::lang::shared_ptr<jubatus::zk>&, const std::string& name,
          unsigned int count_threashold = 1024, unsigned int tick_threshold = 16);
    virtual ~mixer();

    void set_mixer_func(pfi::lang::function<void(const std::vector<std::pair<std::string,int> >&)>);
    // void set_mixer_func2(pfi::lang::function<int(const D&, const D&, D&)>& f){
    //   pfi::concurrent::scoped_lock lk(m_);
    //   mixer_func_ = f;
    // };
    int get_count()const;
    void try_mix();

    static void mixer_loop(void* p);

  private:  
    pfi::lang::function<void(const std::vector<std::pair<std::string,int> >&)> mix_;

    pfi::lang::shared_ptr<jubatus::zk> zk_;
    std::string name_;

    unsigned int count_threshold_;
    unsigned int counter_;
    unsigned int tick_threshold_;
    unsigned int ticktime_;

    pfi::concurrent::thread t_;
    pfi::concurrent::mutex m_;
    pfi::concurrent::condition c_;
    //    pfi::lang::function<int(const D&, const D&, D&)> mixer_func_;
  };
  
  template <typename M, typename D>
  class mixer0{
  public:
    void updated(){
      pfi::concurrent::scoped_lock lk(m_);
      unsigned int new_ticktime = time(NULL);
      ++counter_;
      if(counter_ > count_threshold_ ||
         new_ticktime - ticktime_ > tick_threshold_){
        c_.notify(); // FIXME: need sync here?
      }
    };
    void accessed(){};
    void clear(){
      pfi::concurrent::scoped_lock lk(m_);
      counter_=0;
      ticktime_ = time(NULL);
    };
    void start(){ t_.start(); };
    void get_status(std::map<std::string,std::string>& out){
      pfi::concurrent::scoped_lock lk(m_);
      out["count"] = pfi::lang::lexical_cast<std::string>(counter_);
      out["ticktime"] = pfi::lang::lexical_cast<std::string>(ticktime_); //since last mix
    };

    mixer0(pfi::lang::shared_ptr<jubatus::zk>& z, const std::string& name,
           unsigned int count_threshold, unsigned int tick_threshold)
      :zk_(z),name_(name), 
       count_threshold_(count_threshold),
       counter_(0),
       tick_threshold_(tick_threshold),
       ticktime_(time(NULL)),
       t_(pfi::lang::bind(&mixer::mixer_loop, this))
    {
    };
    virtual ~mixer0(){};

    void set_mixer_func(pfi::lang::function<int(const M*, const D&, const D&, D&)>& f){
      pfi::concurrent::scoped_lock lk(m_);
      mixer_func_ = f;
    };
    int get_count()const {return counter_;} ; //FIXME: not thread-safe
    void try_mix(){
      jubatus::zkmutex zklock(zk_, jubatus::ACTOR_BASE_PATH +"/" + name_ + "/master_lock");
      {
        pfi::concurrent::scoped_lock lk(m_);
        
        c_.wait(m_, 1);
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
      //      mix_(servers);
      // FIXME: do mix here using mixer_func_
      DLOG(INFO) << "....mix done";
    };

    static void mixer_loop(void* p){
      mixer0 * m = static_cast<mixer*>(p);
      while(true) m->try_mix();
    };
    
  private:  
    pfi::lang::function<int(const M*, const D&, const D&, D&)> mixer_func_;

    pfi::lang::shared_ptr<jubatus::zk> zk_;
    std::string name_;

    unsigned int count_threshold_;
    unsigned int counter_;
    unsigned int tick_threshold_;
    unsigned int ticktime_;

    pfi::concurrent::thread t_;
    pfi::concurrent::mutex m_;
    pfi::concurrent::condition c_;
    //    pfi::lang::function<int(const D&, const D&, D&)> mixer_func_;
  };

  static const std::string GET_DIFF("get_diff");
  static const std::string PUT_DIFF("put_diff");
}
