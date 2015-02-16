// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include "system.hpp"

#ifdef __APPLE__
#include <libproc.h>
#endif
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "jubatus/util/lang/exception.h"
#include "jubatus/util/text/json.h"

#include "jubatus/core/common/exception.hpp"
#include "filesystem.hpp"
#include "logger/logger.hpp"

using std::string;
using jubatus::util::lang::lexical_cast;
using jubatus::util::lang::parse_error;

namespace jubatus {
namespace server {
namespace common {

std::string get_program_name() {
  // WARNING: this code will only work on linux or OS X
#ifdef __APPLE__
  char path[PROC_PIDPATHINFO_MAXSIZE];
  int ret = proc_pidpath(getpid(), path, PROC_PIDPATHINFO_MAXSIZE);
#else
  const char* exe_sym_path = "/proc/self/exe";
  // when BSD: /proc/curproc/file
  // when Solaris: /proc/self/path/a.out
  // Unix: getexecname(3)
  char path[PATH_MAX];
  ssize_t ret = readlink(exe_sym_path, path, PATH_MAX);
  if (ret != -1) {
    if (ret == PATH_MAX) {
      throw JUBATUS_EXCEPTION(jubatus::core::common::exception::runtime_error(
            "Failed to get program name. Path size overed PATH_MAX.")
          << jubatus::core::common::exception::error_errno(errno));
    }
    path[ret] = '\0';
  }
#endif
  if (ret < 0) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("Failed to get program name")
      << core::common::exception::error_errno(errno));
  }

  // get basename
  const string program_base_name = common::base_name(path);
  if (program_base_name == path) {
    throw JUBATUS_EXCEPTION(jubatus::core::common::exception::runtime_error(
          string("Failed to get program name from path: ") + path)
        << jubatus::core::common::exception::error_file_name(path));
  }
  return program_base_name;
}

std::string get_user_name() {
  uid_t uid = getuid();
  int64_t buflen = sysconf(_SC_GETPW_R_SIZE_MAX);
  std::vector<char> buf(buflen);
  struct passwd pwd;
  struct passwd* result;
  int ret = getpwuid_r(uid, &pwd, &buf[0], buflen, &result);
  if (ret == 0) {
    if (result != NULL) {
      return result->pw_name;
    }
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("User not found")
      << core::common::exception::error_api_func("getpwuid_r"));
  }
  throw JUBATUS_EXCEPTION(
      jubatus::core::common::exception::runtime_error("Failed to get user name")
      << jubatus::core::common::exception::error_api_func("getpwuid_r")
      << jubatus::core::common::exception::error_errno(ret));
}


int daemonize() {
  return daemon(0, 0);
}

void append_env_path(const string& e, const string& argv0) {
  const char* env = getenv(e.c_str());
  string new_path = string(env) + ":" + argv0;
  setenv(e.c_str(), new_path.c_str(), new_path.size());
}

void append_server_path(const string& argv0) {
  const char* env = getenv("PATH");
  char cwd[PATH_MAX];
  if (!getcwd(cwd, PATH_MAX)) {
    throw JUBATUS_EXCEPTION(
        jubatus::core::common::exception::runtime_error("Failed to getcwd"))
        << jubatus::core::common::exception::error_errno(errno);
  }

  string p = argv0.substr(0, argv0.find_last_of('/'));
  string new_path = string(env) + ":" + cwd + "/" + p + "/../server";
  setenv("PATH", new_path.c_str(), new_path.size());
}

namespace {

string get_statm_path() {
  // /proc/[pid]/statm shows using page size
  char path[64];
  int pid = getpid();  // convert pid_t to int (for "%d")
  snprintf(path, sizeof(path), "/proc/%d/statm", pid);
  return path;
}

}  // namespace

void get_machine_status(machine_status_t& status) {
  // WARNING: this code will only work on linux
  uint64_t vm_virt = 0, vm_rss = 0, vm_shr = 0;

  {
    string path = get_statm_path();
    std::ifstream statm(path.c_str());
    if (statm) {
      const int64_t page_size = sysconf(_SC_PAGESIZE);
      statm >> vm_virt >> vm_rss >> vm_shr;
      vm_virt = vm_virt * page_size / 1024;
      vm_rss = vm_rss * page_size / 1024;
      vm_shr = vm_shr * page_size / 1024;
    }
  }

  // in KB
  status.vm_size = vm_virt;  // total program size(virtual memory)
  status.vm_resident = vm_rss;  // resident set size
  status.vm_share = vm_shr;  // shared
}

}  // namespace common
}  // namespace server
}  // namespace jubatus

