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

#pragma once
#include <string>
#include <queue>
#include <vector>

#include <map>
#include <pficommon/network/mprpc.h>
#include <pficommon/concurrent/mutex.h>

#include "../common/lock_service.hpp"
#include "process.hpp"

namespace jubatus{

class jubervisor{
public:
  jubervisor(const std::string&, int, int = 10, const std::string& logfile = "");
  ~jubervisor();

  int start(std::string, unsigned int);

  // reduces processes to N - FIXME: currently the second value ignored and all processes stopped
  int stop(std::string, unsigned int);
  //  int ensure(std::string);

  void stop_all();
  //  void die_if_deleted(int, int, std::string);
  
private:
  typedef std::vector<process> process_list_t;
  typedef std::map<std::string, process_list_t > child_map_t;

  int start_(const std::string&, unsigned int);
  //  int stop_(const std::string&, std::vector<process>&);

  static void sigchld_handler_(int);

  pfi::lang::shared_ptr<common::lock_service> zk_;
  std::string name_;
  int port_base_;
  std::queue<int> port_pool_;
  child_map_t children_;

  std::string logfile_;
  pfi::concurrent::mutex m_;
  unsigned int max_children_;
};

MPRPC_PROC(start, int(std::string, unsigned int));
MPRPC_PROC(stop, int(std::string, unsigned int));
  //MPRPC_PROC(ensure, int(std::string));

MPRPC_GEN(1, jubervisor, start, stop);
            
}//jubatus
