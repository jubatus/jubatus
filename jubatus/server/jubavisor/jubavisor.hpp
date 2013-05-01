// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_SERVER_JUBAVISOR_JUBAVISOR_HPP_
#define JUBATUS_SERVER_JUBAVISOR_JUBAVISOR_HPP_

#include <map>
#include <queue>
#include <string>
#include <vector>

#include <pficommon/concurrent/mutex.h>

#include "process.hpp"
#include "../common/lock_service.hpp"
#include "../common/mprpc/rpc_util.hpp"
#include "../framework/server_util.hpp"

namespace jubatus {
namespace server {
namespace jubavisor {

class jubavisor {
 public:
  jubavisor(
      const std::string& hosts,
      int port,
      int max = 10,
      const std::string& logfile = "");
  ~jubavisor();

  int start(std::string str, unsigned int N, framework::server_argv argv);

  // reduces processes to N - TODO: currently the second value ignored and all
  // processes stopped
  int stop(std::string str, unsigned int N);
  //  int ensure(std::string);

  void stop_all();
  //  void die_if_deleted(int, int, std::string);

 private:
  typedef std::vector<process> process_list_t;
  typedef std::map<std::string, process_list_t> child_map_t;

  static void atexit_();
  static void sigchld_handler_(int sig);

  int start_(
      const std::string& str,
      unsigned int N,
      const framework::server_argv& argv);
  //  int stop_(const std::string&, std::vector<process>&);

  pfi::lang::shared_ptr<common::lock_service> zk_;
  std::string name_;
  int port_base_;
  std::queue<int> port_pool_;
  child_map_t children_;

  std::string logfile_;
  pfi::concurrent::mutex m_;
  unsigned int max_children_;
};

JUBATUS_MPRPC_PROC(
    start,
    int,
    (std::string, unsigned int, framework::server_argv));
JUBATUS_MPRPC_PROC(stop, int, (std::string, unsigned int));
// MPRPC_PROC(ensure, int(std::string));

JUBATUS_MPRPC_GEN(1, jubavisor, start, stop);

}  // namespace jubavisor
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_JUBAVISOR_JUBAVISOR_HPP_
