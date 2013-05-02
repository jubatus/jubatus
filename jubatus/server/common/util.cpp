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

#include "util.hpp"

#include <arpa/inet.h>
#ifdef __APPLE__
#include <libproc.h>
#endif
#include <net/if.h>
#include <netinet/in.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
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

#include <glog/logging.h>
#include <pficommon/lang/exception.h>
#include <pficommon/text/json.h>

#include "jubatus/core/common/exception.hpp"

using std::string;
using pfi::lang::lexical_cast;
using pfi::lang::parse_error;

namespace jubatus {
namespace util {

// TODO(kashihara): AF_INET does not specify IPv6
void get_ip(const char* nic, string& out) {
  int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
          "Failed to create socket(AF_INET, SOCK_DGRAM)")
        << jubatus::exception::error_errno(errno));
  }

  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name, nic, IFNAMSIZ - 1);
  if (ioctl(fd, SIOCGIFADDR, &ifr) == -1) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
          "Failed to get IP address from interface")
        << jubatus::exception::error_errno(errno));
  }
  close(fd);

  struct sockaddr_in* sin = (struct sockaddr_in*) (&(ifr.ifr_addr));
  out = inet_ntoa((struct in_addr) (sin->sin_addr));
}

string get_ip(const char* nic) {
  string ret;
  get_ip(nic, ret);
  return ret;
}

string base_name(const string& path) {
  size_t found = path.rfind('/');
  return found != string::npos ? path.substr(found + 1) : path;
}

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
      throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
            "Failed to get program name. Path size overed PATH_MAX.")
          << jubatus::exception::error_errno(errno));
    }
    path[ret] = '\0';
  }
#endif
  if (ret < 0) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("Failed to get program name")
        << jubatus::exception::error_errno(errno));
  }

  // get basename
  const string program_base_name = base_name(path);
  if (program_base_name == path) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
          string("Failed to get program name from path: ") + path)
        << jubatus::exception::error_file_name(path));
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
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("User not found")
        << jubatus::exception::error_api_func("getpwuid_r"));
  }
  throw JUBATUS_EXCEPTION(
      jubatus::exception::runtime_error("Failed to get user name")
      << jubatus::exception::error_api_func("getpwuid_r")
      << jubatus::exception::error_errno(ret));
}

bool is_writable(const char* dir_path) {
  struct stat st_buf;
  if (stat(dir_path, &st_buf) < 0) {
    return false;
  }

  if (!S_ISDIR(st_buf.st_mode)) {
    errno = ENOTDIR;
    return false;
  }

  if (access(dir_path, W_OK) < 0) {
    return false;
  }

  return true;
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
        jubatus::exception::runtime_error("Failed to getcwd"))
        << jubatus::exception::error_errno(errno);
  }

  string p = argv0.substr(0, argv0.find_last_of('/'));
  string new_path = string(env) + ":" + cwd + "/" + p + "/../server";
  setenv("PATH", new_path.c_str(), new_path.size());
}

void get_machine_status(machine_status_t& status) {
  // WARNING: this code will only work on linux
  try {
    // /proc/[pid]/statm shows using page size
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/statm", getpid());
    std::ifstream statm(path);

    const int64_t page_size = sysconf(_SC_PAGESIZE);
    uint64_t vm_virt, vm_rss, vm_shr;
    statm >> vm_virt >> vm_rss >> vm_shr;
    vm_virt = vm_virt * page_size / 1024;
    vm_rss = vm_rss * page_size / 1024;
    vm_shr = vm_shr * page_size / 1024;

    // in KB
    status.vm_size = vm_virt;  // total program size(virtual memory)
    status.vm_resident = vm_rss;  // resident set size
    status.vm_share = vm_shr;  // shared
  } catch (...) {
    // store zero
    status.vm_size = 0;
    status.vm_resident = 0;
    status.vm_share = 0;
  }
}

namespace {

void exit_on_term(int /* signum */) {
  LOG(INFO) << "stopping RPC server";
  exit(0);
}

}  // namespace

void set_exit_on_term() {
  struct sigaction sigact;
  sigact.sa_handler = exit_on_term;
  sigact.sa_flags = SA_RESTART;

  if (sigaction(SIGTERM, &sigact, NULL) != 0) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("can't set SIGTERM handler")
        << jubatus::exception::error_api_func("sigaction")
        << jubatus::exception::error_errno(errno));
  }

  if (sigaction(SIGINT, &sigact, NULL) != 0) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("can't set SIGINT handler")
        << jubatus::exception::error_api_func("sigaction")
        << jubatus::exception::error_errno(errno));
  }
}

void ignore_sigpipe() {
  // portable code for socket write(2) MSG_NOSIGNAL
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("can't ignore SIGPIPE")
        << jubatus::exception::error_api_func("signal")
        << jubatus::exception::error_errno(errno));
  }
}

}  // namespace util
}  // namespace jubatus

