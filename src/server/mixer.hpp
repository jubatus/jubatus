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
#include "storage/storage_base.hpp"

#ifdef HAVE_ZOOKEEPER_H
#  include "../common/zk.hpp"
#endif

#include <pficommon/concurrent/thread.h>
#include <pficommon/concurrent/mutex.h>
#include <pficommon/concurrent/condition.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/function.h>

namespace jubatus{

  class jubatus_classifier_serv;

  class mixer{
  public:
    void updated();
    void accessed();
    void clear();
    void start();
    void get_status(std::map<std::string,std::string>&);

    mixer(pfi::lang::shared_ptr<jubatus::zk>&, const std::string& name,
          pfi::lang::function<void(const std::vector<std::pair<std::string,int> >&)> mix,
          unsigned int count_threashold = 1024, unsigned int tick_threshold = 16);
    virtual ~mixer();

    void set_mixer_func(pfi::lang::function<void(const std::vector<std::pair<std::string,int> >&)>);
    int get_count()const;
    void try_mix();

    static void mixer_loop(void* p);
    static void dummy(const std::vector<std::pair<std::string,int> >&);

    // [server] -> ()
    // (a -> b) -> (b -> b -> b) -> (b -> c) -> (c -> d) -> ()
    template<typename A, typename B, typename C, typename D>
    void mix(const std::vector<std::pair<std::string, int> >& servers) const ;


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

  };

  static const std::string GET_DIFF("get_diff");
  static const std::string PUT_DIFF("put_diff");

}
