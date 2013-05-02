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

#include <errno.h>
#include <sys/wait.h>

#include <algorithm>
#include <csignal>
#include <string>

#include <glog/logging.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/lang/bind.h>

#include "jubavisor.hpp"
#include "jubatus/core/common/exception.hpp"
#include "../common/membership.hpp"
#include "../common/network.hpp"
#include "../common/util.hpp"

using pfi::concurrent::scoped_lock;
using pfi::lang::bind;

namespace jubatus {
namespace server {
namespace jubavisor {

using common::create_lock_service;
using common::build_loc_str;

namespace {
jubavisor* g_jubavisor = NULL;

// for GCC and Clang compatibility:
// to use pfi::lang::bind 'void (*)(int) __attribute__((noreturn))'
void exit_wrapper(int status) {
  exit(status);
}
}  // namespace

jubavisor::jubavisor(
    const std::string& hosts,
    int port,
    int max,
    const std::string& logfile)
    : zk_(create_lock_service("zk", hosts, 1024, logfile)),
      port_base_(port),
      logfile_(logfile),
      max_children_(max) {
  jubatus::util::ignore_sigpipe();
  jubatus::util::set_exit_on_term();
  ::atexit(jubavisor::atexit_);

  // handle SIG_CHLD
  struct sigaction sa;
  sa.sa_handler = sigchld_handler_;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("failed sigaction(SIGCHLD)")
        << jubatus::exception::error_errno(errno));
  }

  zk_->create(jubatus::server::common::JUBATUS_BASE_PATH, "");
  zk_->create(jubatus::server::common::JUBAVISOR_BASE_PATH, "");
  zk_->create(jubatus::server::common::ACTOR_BASE_PATH, "");

  name_ =
    build_loc_str(jubatus::server::common::get_default_v4_address(), port);
  std::string path = jubatus::server::common::JUBAVISOR_BASE_PATH + "/" + name_;
  zk_->create(path, "", true);
  // TODO(kumagi):
  //  pfi::lang::function<void(int,int,std::string)> f
  //    = bind(&jubavisor::die_if_deleted, this, _1, _2, _3);
  //  zk_->bind_watcher(path, f);
  DLOG(INFO) << path << " created.";

  for (unsigned int i = 0; i < max_children_; ++i) {
    port_pool_.push(++port_base_);
  }

  pfi::lang::function<void()> h = bind(&jubavisor::stop_all, this);
  zk_->push_cleanup(h);
  pfi::lang::function<void()> g = bind(&exit_wrapper, -1);
  zk_->push_cleanup(g);

  g_jubavisor = this;
}

jubavisor::~jubavisor() {
  stop_all();
}

void jubavisor::atexit_() {
  if (g_jubavisor && g_jubavisor->zk_) {
    g_jubavisor->zk_->force_close();
  }
}

void jubavisor::sigchld_handler_(int sig) {
  pid_t child_pid;
  int child_status;

  // This handler need to waitpid(-1, ,WNOHANG) to destroy whole zombie process
  // correctly, because signal hander cannot handle some signal on the same
  // time.
  while ((child_pid = waitpid(-1, &child_status, WNOHANG)) > 0) {
    if (!g_jubavisor) {
      continue;
    }

    try {
      scoped_lock lk(g_jubavisor->m_);

      for (child_map_t::iterator it = g_jubavisor->children_.begin(),
               end = g_jubavisor->children_.end(); it != end; ++it) {
        // find child_pid from jubavisor
        bool released = false;
        for (process_list_t::iterator process_it = it->second.begin(),
                 end = it->second.end(); process_it != end; ++process_it) {
          if (child_pid != process_it->get_pid()) {
            continue;
          }

          LOG(INFO) << process_it->get_server()
                    << " with port " << process_it->get_rpc_port()
                    << " exited pid: " << child_pid;

          // release reserved port
          g_jubavisor->port_pool_.push(process_it->get_rpc_port());
          it->second.erase(process_it);

          released = true;
          break;
        }
        if (released) {
          break;
        }
      }
    } catch (...) {
    }
  }
}

// str : "<server>/<name>
// server : "jubaclassifier" ...
// name : any but ""
// -> exec ./<server> -n <name> -p <rpc_port> -z <zk>
int jubavisor::start(
    std::string str,
    unsigned int N,
    framework::server_argv argv) {
  scoped_lock lk(m_);
  LOG(INFO) << str << " " << N;
  return start_(str, N, argv);
}

int jubavisor::start_(
    const std::string& str,
    unsigned int N,
    const framework::server_argv& argv) {
  std::string name;
  {
    process p(zk_->get_hosts());
    if (!p.set_names(str)) {
      LOG(ERROR) << "cannot parse " << str;
      return -1;
    }
    name = p.get_name();
  }
  child_map_t::iterator it = children_.find(name);
  if (it == children_.end()) {
    children_.insert(make_pair(name, process_list_t()));
  }
  it = children_.find(name);
  if (it->second.size() > N) {
    LOG(ERROR) << it->second.size() << " " << name
               << " already running at this machine.";
    return -1;
  }
  N -= it->second.size();

  if (port_pool_.size() < N) {
    LOG(ERROR) << "cannot spawn more than " << max_children_ << " processes.";
    LOG(ERROR) << "currently remaining " << port_pool_.size() << " while " << N
               << " requested.";
    return -1;
  }

  for (unsigned int n = 0; n < N; ++n) {
    process p(zk_->get_hosts(), argv);
    p.set_names(str);
    it = children_.find(name);

    int p1 = port_pool_.front();
    port_pool_.pop();
    if (p.spawn_link(p1)) {
      it->second.push_back(p);

    } else {
      LOG(ERROR) << "cannot start: " << p.get_name();
      port_pool_.push(p1);
      return -1;
    }
  }
  return 0;
}

int jubavisor::stop(std::string str, unsigned int N) {
  DLOG(ERROR) << str;
  process p(zk_->get_hosts());
  p.set_names(str);
  process_list_t v;
  {
    scoped_lock lk(m_);
    child_map_t::iterator it = children_.find(p.get_name());
    if (it == children_.end()) {
      LOG(INFO) << p.get_name() << " is not in children.";
      return 0;
    }
    v = it->second;
    children_.erase(it);
  }
  int r = 0;

  for (size_t i = 0; i < v.size(); ++i) {
    if (!v[i].kill()) {
      r--;
    } else {
      port_pool_.push(v[i].get_rpc_port());
    }
  }
  return r;
}

void jubavisor::stop_all() {
  DLOG(ERROR) << __func__;
  scoped_lock lk(m_);

  for (child_map_t::iterator it = children_.begin(), end = children_.end();
       it != end; ++it) {
    for (size_t i = 0; i < it->second.size(); ++i) {
      if (!it->second[i].kill()) {
        LOG(ERROR) << "failed stopping process: name("
                   << it->second[i].get_name() << ") server("
                   << it->second[i].get_server() << ") ";
        perror(__func__);
      }
    }
  }
  children_.clear();
}

}  // namespace jubavisor
}  // namespace server
}  // namespace jubatus
