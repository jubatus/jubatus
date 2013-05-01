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

#include "zk.hpp"

#include <assert.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <vector>
#include <pficommon/concurrent/lock.h>
#include <pficommon/lang/bind.h>
#include <pficommon/data/string/utility.h>
#include "jubatus/core/common/exception.hpp"

using pfi::concurrent::scoped_lock;
using std::vector;
using std::string;

namespace jubatus {
namespace server {
namespace common {

zk::zk(const string& hosts, int timeout, const string& logfile)
    : zh_(NULL),
      hosts_(hosts),
      logfilep_(NULL) {
  if (logfile != "") {
    logfilep_ = fopen(logfile.c_str(), "a+");
    if (!logfilep_) {
      throw JUBATUS_EXCEPTION(
          jubatus::exception::runtime_error("cannot open zk logfile")
          << jubatus::exception::error_file_name(logfile.c_str())
          << jubatus::exception::error_errno(errno)
          << jubatus::exception::error_api_func("fopen"));
    }
    zoo_set_log_stream(logfilep_);
  }

  zh_ = zookeeper_init(hosts.c_str(), NULL, timeout * 1000, 0, NULL, 0);
  if (!zh_) {
    perror("");
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("failed to initialize zk: " + hosts)
        << jubatus::exception::error_api_func("zookeeper_init")
        << jubatus::exception::error_errno(errno));
  }

  // sleep the state got not ZOO_CONNECTING_STATE
  while ((state_ = zoo_state(zh_)) == ZOO_CONNECTING_STATE) {
    usleep(100);
  }

  if (is_unrecoverable(zh_) == ZINVALIDSTATE) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("cannot connect zk:" + hosts)
        << jubatus::exception::error_api_func("is_unrecoverable")
        << jubatus::exception::error_message(zerror(errno)));
  }

  zoo_set_context(zh_, this);
  zoo_set_watcher(zh_, mywatcher);
}

zk::~zk() {
  force_close();
  if (logfilep_) {
    fclose(logfilep_);
    logfilep_ = NULL;
  }
}

void zk::force_close() {
  if (zh_) {
    zookeeper_close(zh_);
    zh_ = NULL;
  }
}

bool zk::create(const string& path, const string& payload, bool ephemeral) {
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

bool zk::set(const string& path, const string& payload) {
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
bool zk::create_seq(const string& path, string& seqfile) {
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

bool zk::create_id(const string& path, uint32_t prefix, uint64_t& res) {
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

bool zk::remove(const string& path) {
  scoped_lock lk(m_);
  int rc = zoo_delete(zh_, path.c_str(), -1);
  if (rc != ZOK && rc != ZNONODE) {
    LOG(ERROR) << "failed to remove: " << path << " - " << zerror(rc);
    return false;
  }

  DLOG(INFO) << __func__ << " " << path;
  return true;
}

bool zk::exists(const string& path) {
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
  pfi::lang::function<void(int, int, string)>* fp =
      static_cast<pfi::lang::function<void(int, int, string)>*>(watcherCtx);
  (*fp)(type, state, string(path));
  delete fp;
}

bool zk::bind_watcher(
    const string& path,
    pfi::lang::function<void(int, int, string)>& f) {
  pfi::lang::function<void(int, int, string)>* fp = new pfi::lang::function<
      void(int, int, string)>(f);
  int rc = zoo_wexists(zh_, path.c_str(), my_znode_watcher, fp, NULL);
  return rc == ZOK;
}

bool zk::list(const string& path, vector<string>& out) {
  scoped_lock lk(m_);
  return list_(path, out);
}

bool zk::list_(const string& path, vector<string>& out) {
  struct String_vector s;
  int rc = zoo_get_children(zh_, path.c_str(), 0, &s);
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

bool zk::hd_list(const string& path, string& out) {
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

bool zk::read(const string& path, string& out) {
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

void zk::push_cleanup(pfi::lang::function<void()>& f) {
  scoped_lock lk(m_);
  cleanups_.push_back(f);
}

void zk::run_cleanup() {
  scoped_lock lk(m_);
  for (size_t i = 0; i < cleanups_.size(); ++i) {
    cleanups_[i]();
  }
}

const string& zk::get_hosts() const {
  return hosts_;
}

const string zk::type() const {
  return "zk";
}

bool zkmutex::lock() {
  pfi::concurrent::scoped_lock lk(m_);
  LOG(ERROR) << "not implemented: " << __func__;
  while (!has_lock_) {
    if (try_lock()) {
      break;
    }
    sleep(1);
  }

  return true;
}

bool zkmutex::try_lock() {
  pfi::concurrent::scoped_lock lk(m_);
  if (has_lock_) {
    return has_lock_;
  }

  string prefix = path_ + "/wlock_";
  if (!zk_.create_seq(prefix, seqfile_)) {
    return false;
  }

  if (seqfile_ == "") {
    return false;
  }

  vector<string> list;
  if (!zk_.list(path_, list) || list.empty()) {
    return false;
  }

  has_lock_ = true;
  for (size_t i = 0; i < list.size(); i++) {
    // not exist all lock less than me.
    string path1((path_ + '/' + list[i]).c_str(), prefix.size() + 16);
    if (seqfile_.compare(prefix.length(), -1, path1,
                         prefix.length(), -1) > 0) {
      has_lock_ = false;
      break;
    }
  }

  if (!has_lock_) {
    zk_.remove(seqfile_);
  } else {
    DLOG(INFO) << "got write lock: " << seqfile_;
  }

  return has_lock_;
}

bool zkmutex::unlock() {
  pfi::concurrent::scoped_lock lk(m_);
  if (has_lock_) {
    return zk_.remove(seqfile_);
  }
  return true;
}

bool zkmutex::rlock() {
  pfi::concurrent::scoped_lock lk(m_);
  LOG(ERROR) << "not implemented: " << __func__;
  while (!has_rlock_) {
    if (try_rlock()) {
      break;
    }
    sleep(1);
  }

  return true;
}

bool zkmutex::try_rlock() {
  pfi::concurrent::scoped_lock lk(m_);
  if (has_rlock_) {
    return has_rlock_;
  }

  string prefix = path_ + "/rlock_";
  if (!zk_.create_seq(prefix, seqfile_)) {
    return false;
  }

  if (seqfile_ == "") {
    return false;
  }

  vector<string> list;
  if (!zk_.list(path_, list) || list.empty()) {
    return false;
  }

  has_rlock_ = true;
  for (size_t i = 0; i < list.size(); i++) {
    // not exist write lock less than me.
    if (pfi::data::string::starts_with(list[i], string("wlock_"))) {
      string path1((path_ + '/' + list[i]).c_str(), prefix.size() + 16);
      if (seqfile_.compare(prefix.length(), -1, path1,
                           prefix.length(), -1) > 0) {
        has_rlock_ = false;
        break;
      }
    }
  }

  if (!has_rlock_) {
    zk_.remove(seqfile_);
  } else {
    DLOG(INFO) << "got read lock: " << seqfile_;
  }

  return has_rlock_;
}

bool zkmutex::unlock_r() {
  pfi::concurrent::scoped_lock lk(m_);
  if (has_rlock_) {
    return zk_.remove(seqfile_);
  }
  return true;
}

void mywatcher(zhandle_t* zh, int type, int state, const char* path, void* p) {
  zk* zk_ = static_cast<zk*>(p);
  // ZOO_* cannot use switch because it const int.
  if (type == ZOO_CREATED_EVENT) {
  } else if (type == ZOO_DELETED_EVENT) {
  } else if (type == ZOO_CHANGED_EVENT) {
  } else if (type == ZOO_CHILD_EVENT) {
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
