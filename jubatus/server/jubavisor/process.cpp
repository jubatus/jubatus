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

#include "process.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include <glog/logging.h>
#include <pficommon/lang/cast.h>

#include "jubatus/core/common/exception.hpp"
#include "../common/util.hpp"

using pfi::lang::lexical_cast;

namespace jubatus {
namespace server {
namespace jubavisor {

process::process(const std::string& zkhosts)
    : zk_hosts_(zkhosts) {
}

process::process(
    const std::string& zkhosts,
    const framework::server_argv& server_option)
    : zk_hosts_(zkhosts),
      server_option_(server_option) {
}

process::~process() {
}

// str : "<server>/<name>
bool process::set_names(const std::string& str) {
  if (str.find(" ", 0) != std::string::npos) {
    return false;
  }
  server_ = str.substr(0, str.find_first_of("/"));
  name_ = str.substr(str.find_first_of("/") + 1);

  return !(server_.empty() || name_.empty());
}

// open file and connect fd to the new file handle
void redirect(const char* filename, int fd) {
  int new_fd = open(filename, O_WRONLY | O_CREAT, 0644);
  if (new_fd < 0) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("cannot open file"));
  }
  int r = dup2(new_fd, fd);
  if (r < 0) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("cannot dup(2)"));
  }
  close(new_fd);
}

bool process::spawn_link(int p) {
  // TODO(kumagi): set better path in installation or else
  // set $PATH where you can find jubatus servers
  std::string cmd = server_;  // TODO(suma): check cmd exits
  LOG(INFO) << "forking " << cmd << " with port " << p;

  server_option_.port = p;
  pid_ = fork();
  if (pid_ > 0) {
    LOG(INFO) << "forked - pid: " << pid_;
    return true;

  } else if (pid_ == 0) {
    redirect("/dev/null", 1);
    redirect("/dev/null", 2);
    const std::string argv[] = {
      cmd,
      "-z", zk_hosts_,
      "-n", name_,
      "-p", lexical_cast<std::string>(p),
      "-B", server_option_.bind_if,
      "-c", lexical_cast<std::string>(server_option_.threadnum),
      "-t", lexical_cast<std::string>(server_option_.timeout),
      "-d", server_option_.datadir,
      "-l", server_option_.logdir,
      "-e", lexical_cast<std::string, int>(server_option_.loglevel),
      "-s", lexical_cast<std::string, int>(server_option_.interval_sec),
      "-i", lexical_cast<std::string, int>(server_option_.interval_count),
    };
    std::vector<const char*> arg_list;
    for (size_t i = 0; i < sizeof(argv) / sizeof(*argv); ++i) {
      arg_list.push_back(argv[i].c_str());
    }
    if (server_option_.join) {
      arg_list.push_back("-j");
    }
    arg_list.push_back(NULL);

    execvp(cmd.c_str(), (char* const *) &arg_list[0]);
    perror(cmd.c_str());  // execv only returns on error

  } else {
    perror("failed on forking new process");
    perror(cmd.c_str());
    LOG(ERROR) << cmd;
    LOG(ERROR) << getenv("PATH");
    LOG(ERROR) << strerror(errno);
    return false;
  }

  assert(false);
  // never reaches here
  return false;
}

bool process::kill() {
  int r = ::kill(pid_, SIGTERM);
  if (r != 0) {
    perror("cannot kill");
    return false;
  }
  int status = 0;
  waitpid(pid_, &status, 0);

  if (WCOREDUMP(status)) {
    LOG(ERROR) << name_ << " coredumped.";
  }  // TODO(kumagi): add more error processing

  DLOG(INFO) << "stopped: " << pid_;
  DLOG(ERROR) << "stopped: " << pid_;
  return true;
}

bool process::has_samespec(const process& rhs) const {
  return (get_name() == rhs.get_name() and get_server() == rhs.get_server());
}

}  // namespace jubavisor
}  // namespace server
}  // namespace jubatus
