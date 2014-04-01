// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "burst_zk.hpp"
#include "burst_distributed_storage.hpp"

#include <unistd.h>
#include <algorithm>
#include <string>
#include <vector>
#include "jubatus/util/concurrent/lock.h"
#include "jubatus/util/lang/bind.h"
#include "jubatus/util/data/string/utility.h"
#include "../common/exception.hpp"

using jubatus::util::concurrent::scoped_lock;
using jubatus::util::lang::shared_ptr;
using jubatus::util::lang::function;
using std::vector;
using std::string;

namespace jubatus {
namespace server {
namespace common {

void burst_mywatcher(
    zhandle_t* zh, int type, int state, const char* path, void* p);

burst_zk::burst_zk(const string& hosts, int timeout, const string& logfile)
    : zh_(NULL),
      hosts_(hosts),
      logfilep_(NULL) {
  if (logfile != "") {
    logfilep_ = fopen(logfile.c_str(), "a+");
    if (!logfilep_) {
      throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("cannot open burst_zk logfile")
        << core::common::exception::error_file_name(logfile.c_str())
        << core::common::exception::error_errno(errno)
        << core::common::exception::error_api_func("fopen"));
    }
    zoo_set_log_stream(logfilep_);
  }

  zh_ = zookeeper_init(hosts.c_str(), NULL, timeout * 1000, 0, NULL, 0);
  if (!zh_) {
    perror("");
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error(
        "failed to initialize burst_zk: " + hosts)
      << core::common::exception::error_api_func("zookeeper_init")
      << core::common::exception::error_errno(errno));
  }

  // sleep the state got not ZOO_CONNECTING_STATE
  while ((state_ = zoo_state(zh_)) == ZOO_CONNECTING_STATE) {
    usleep(100);
  }

  if (is_unrecoverable(zh_) == ZINVALIDSTATE) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("cannot connect burst_zk:" + hosts)
      << core::common::exception::error_api_func("is_unrecoverable")
      << core::common::exception::error_message(zerror(errno)));
  }

  zoo_set_context(zh_, this);
  zoo_set_watcher(zh_, burst_mywatcher);
}

burst_zk::~burst_zk() {
  force_close();
  if (logfilep_) {
    fclose(logfilep_);
    logfilep_ = NULL;
  }
}

void burst_zk::force_close() {
  if (zh_) {
    zookeeper_close(zh_);
    zh_ = NULL;
  }
}

bool burst_zk::create(
    const string& path, const string& payload, bool ephemeral) {
  scoped_lock lk(m_);
  int rc = zoo_create(zh_, path.c_str(), payload.c_str(), payload.length(),
                      &ZOO_OPEN_ACL_UNSAFE,
                      ((ephemeral) ? ZOO_EPHEMERAL : 0),  // | ZOO_SEQUENCE
                      NULL, 0);
  if (ephemeral) {
    if (rc != ZOK) {
      LOG(ERROR) << "failed to create: " << path << " - " << zerror(rc);
      return false;
    }
  } else {
    if (rc != ZOK && rc != ZNODEEXISTS) {
      LOG(ERROR) << "failed to create: " << path << " - " << zerror(rc);
      return false;
    }
  }

  DLOG(INFO) << __func__ << " " << path;
  return true;
}

bool burst_zk::set(const string& path, const string& payload) {
  scoped_lock lk(m_);
  int rc = zoo_set(zh_, path.c_str(), payload.c_str(), payload.length(), -1);
  if (rc != ZOK) {
    LOG(ERROR) << path << " failed in setting " << rc << " " << zerror(rc);
    return false;
  }
  DLOG(INFO) << __func__ << " " << path << " - " << payload;
  return true;
}

// "/some/path" => "/some/path0000012"
bool burst_zk::create_seq(const string& path, string& seqfile) {
  scoped_lock lk(m_);
  string path_buffer(path.size() + 16, '\0');
  int rc = zoo_create(zh_, path.c_str(), NULL, 0, &ZOO_OPEN_ACL_UNSAFE,
                      ZOO_EPHEMERAL | ZOO_SEQUENCE, &path_buffer[0],
                      path.size() + 16);
  seqfile = "";
  if (rc != ZOK) {
    LOG(ERROR) << "failed to create: " << path << " - " << zerror(rc);
    return false;
  } else {
    seqfile = path_buffer;
    DLOG(INFO) << __func__ << " " << seqfile;
    return true;
  }
}

bool burst_zk::create_id(const string& path, uint32_t prefix, uint64_t& res) {
  scoped_lock lk(m_);
  struct Stat st;
  int rc = zoo_set2(zh_, path.c_str(), "dummy", 6, -1, &st);

  if (rc != ZOK) {
    LOG(ERROR) << "failed to set data: " << path << " - " << zerror(rc);
    return false;
  }

  res = (static_cast<uint64_t>(prefix) << 32) | st.version;
  DLOG(INFO) << __func__ << " " << path;
  return true;
}

bool burst_zk::remove(const string& path) {
  scoped_lock lk(m_);
  int rc = zoo_delete(zh_, path.c_str(), -1);
  if (rc != ZOK && rc != ZNONODE) {
    LOG(ERROR) << "failed to remove: " << path << " - " << zerror(rc);
    return false;
  }

  DLOG(INFO) << __func__ << " " << path;
  return true;
}

bool burst_zk::exists(const string& path) {
  scoped_lock lk(m_);
  int rc = zoo_exists(zh_, path.c_str(), 0, NULL);
  return rc == ZOK;
}

void my_znode_watcher(
    zhandle_t* zh,
    int type,
    int state,
    const char* path,
    void* watcherCtx) {
  function<void(int, int, string)>* fp =
      static_cast<function<void(int, int, string)>*>(watcherCtx);
  try {
    (*fp)(type, state, string(path));
  } catch(const std::exception& e) {
    LOG(WARNING) << "exception thrown from zk watcher callback: " << e.what();
  } catch (...) {
    LOG(WARNING) << "unknown exception thrown from zk watcher callback";
  }
  delete fp;
}

bool burst_zk::bind_watcher(
    const string& path,
    function<void(int, int, string)>& f) {
  function<void(int, int, string)>* fp =
      new function<void(int, int, string)>(f);
  int rc = zoo_wexists(zh_, path.c_str(), my_znode_watcher, fp, NULL);
  return rc == ZOK;
}

void my_znode_delete_watcher(
    zhandle_t* zh,
    int type,
    int state,
    const char* path,
    void* watcherCtx) {
  // state should be checked?
  if (type == ZOO_DELETED_EVENT) {
    function<void(string)>* fp =
        static_cast<function<void(string)>*>(watcherCtx);
    try {
      (*fp)(string(path));
    } catch(const std::exception& e) {
      LOG(WARNING) << "exception thrown from zk watcher callback: " << e.what();
    } catch (...) {
      LOG(WARNING) << "unknown exception thrown from zk watcher callback";
    }
    delete fp;
  } else {
    // if not delete event, re-register
    DLOG(INFO)
        << "non-delete event happen in path:["
        << path << "] type:["
        << type << "] state:["
        << state << "]";
    int rc = zoo_wexists(zh, path, my_znode_delete_watcher, watcherCtx, NULL);
    if (rc != ZOK) {
      LOG(WARNING) << "cannot watch the path: " << path;
    }
  }
}

bool burst_zk::bind_delete_watcher(
    const string& path,
    function<void(string)>& f) {
  function<void(string)>* fp = new function<void(string)>(f);
  int rc = zoo_wexists(zh_, path.c_str(), my_znode_delete_watcher, fp, NULL);
  return rc == ZOK;
}

bool burst_zk::list(const string& path, vector<string>& out) {
  scoped_lock lk(m_);
  return list_(path, out);
}

bool burst_zk::list_(const string& path, vector<string>& out) {
  struct String_vector s;
  // int rc = zoo_get_children(zh_, path.c_str(), 0, &s);
  // 3rd arg 1 -> put up watch flag
  int rc = zoo_get_children(zh_, path.c_str(), 1, &s);
  if (rc == ZOK) {
    for (int i = 0; i < s.count; ++i) {
      out.push_back(s.data[i]);  // full path => #{path}/#{s.data[i]}
    }
    std::sort(out.begin(), out.end());
    return true;
  } else if (rc == ZNONODE) {
    return true;
  } else {
    LOG(ERROR) << "failed to get children: " << path << " - " << zerror(rc);
    return false;
  }
}

bool burst_zk::hd_list(const string& path, string& out) {
  struct String_vector s;
  scoped_lock lk(m_);
  int rc = zoo_get_children(zh_, path.c_str(), 0, &s);
  if (rc == ZOK) {
    if (0 < s.count) {
      out = s.data[0];
    }
    return true;
  }
  LOG(ERROR) << "failed to get children: " << path << " - " << zerror(rc);
  return false;
}

bool burst_zk::read(const string& path, string& out) {
  scoped_lock lk(m_);
  char buf[1024];
  int buflen = 1024;
  int rc = zoo_get(zh_, path.c_str(), 0, buf, &buflen, NULL);
  if (rc == ZOK) {
    out = string(buf, buflen);
    return buflen <= 1024;
  } else {
    LOG(ERROR) << "failed to get data: " << path << " - " << zerror(rc);
    return false;
  }
}

void burst_zk::push_cleanup(const function<void()>& f) {
  scoped_lock lk(m_);
  cleanups_.push_back(f);
}

void burst_zk::run_cleanup() {
  scoped_lock lk(m_);
  for (size_t i = 0; i < cleanups_.size(); ++i) {
    cleanups_[i]();
  }
}

const string& burst_zk::get_hosts() const {
  return hosts_;
}

const string burst_zk::type() const {
  return "zk";
}

const std::string burst_zk::get_connected_host_and_port() const {
  std::stringstream ret;
  struct sockaddr sock_addr;
  socklen_t sock_len = sizeof(sock_addr);

  // This return NULL when state is not ZOO_CONNECTED_STATE
  if (zookeeper_get_connected_host(zh_, &sock_addr, &sock_len) == NULL) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("cannot get connected host")
      << core::common::exception::error_api_func("zookeeper_get_connected_host")
      << core::common::exception::error_message(zerror(errno)));
  }

  // TODO(@gintenlabo): when zk's original part is added to common::util, use it
  if (sock_addr.sa_family == AF_INET) {  // Only IPv4
      struct sockaddr_in* sock_addr_in = (struct sockaddr_in *) &sock_addr;
      char host[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(sock_addr_in->sin_addr), host, sizeof(host));
      ret << host << ":" << ntohs(sock_addr_in->sin_port);
  }

  return ret.str();
}

void burst_zk::put_up_child_event_flag(const std::string& path) {
  std::vector<std::string> out;
  list(path, out);
}

void burst_mywatcher(
    zhandle_t* zh, int type, int state, const char* path, void* p) {
  burst_zk* zk_ = static_cast<burst_zk*>(p);
  DLOG(INFO) << "burst_mywatcher: start";
  if (type == ZOO_CREATED_EVENT) {
  } else if (type == ZOO_DELETED_EVENT) {
  } else if (type == ZOO_CHANGED_EVENT) {
  } else if (type == ZOO_CHILD_EVENT) {
    DLOG(INFO) << "ZOO_CHILD_EVENT path:"
               << path;
    using jubatus::core::burst::storage::burst_distributed_storage;
    shared_ptr<burst_distributed_storage> bs =
        burst_distributed_storage::get_instance();
    bs->do_after_notify_impl();
  } else if (type == ZOO_SESSION_EVENT) {
    if (state != ZOO_CONNECTED_STATE && state != ZOO_ASSOCIATING_STATE) {
      LOG(ERROR) << "zk connection expiration - type: " << type << ", state: "
                 << state;
      zk_->run_cleanup();
    }
  } else if (type == ZOO_NOTWATCHING_EVENT) {
  } else {
    LOG(ERROR) << "unknown event type - type: " << type << ", state: " << state;
  }
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
