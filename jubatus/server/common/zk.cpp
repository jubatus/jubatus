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

#include "zk.hpp"

#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include "jubatus/util/concurrent/lock.h"
#include "jubatus/util/data/string/utility.h"
#include "jubatus/util/lang/noncopyable.h"
#include "jubatus/core/common/assert.hpp"
#include "jubatus/core/common/exception.hpp"

using jubatus::util::concurrent::scoped_lock;
using std::vector;
using std::string;

namespace {

const char* zk_type2str(int type) {
  if (type == ZOO_CREATED_EVENT) {
    return "CREATED_EVENT";
  } else if (type == ZOO_DELETED_EVENT) {
    return "DELETED_EVENT";
  } else if (type == ZOO_CHANGED_EVENT) {
    return "CHANGED_EVENT";
  } else if (type == ZOO_CHILD_EVENT) {
    return "CHILD_EVENT";
  } else if (type == ZOO_SESSION_EVENT) {
    return "SESSION_EVENT";
  } else if (type == ZOO_NOTWATCHING_EVENT) {
    return "NOTWATCHING_EVENT";
  }

  return "UNKNOWN_EVENT_TYPE";
}

const char* zk_state2str(int state) {
  if (state == 0) {
    return "CLOSED_STATE";
  } else if (state == ZOO_CONNECTING_STATE) {
    return "CONNECTING_STATE";
  } else if (state == ZOO_ASSOCIATING_STATE) {
    return "ASSOCIATING_STATE";
  } else if (state == ZOO_CONNECTED_STATE) {
    return "CONNECTED_STATE";
  } else if (state == ZOO_EXPIRED_SESSION_STATE) {
    return "EXPIRED_SESSION_STATE";
  } else if (state == ZOO_AUTH_FAILED_STATE) {
    return "AUTH_FAILED_STATE";
  }

  return "INVALID_STATE";
}

}  // namespace

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
          core::common::exception::runtime_error("cannot open zk logfile")
          << core::common::exception::error_file_name(logfile.c_str())
          << core::common::exception::error_errno(errno)
          << core::common::exception::error_api_func("fopen"));
    }
    zoo_set_log_stream(logfilep_);
  }

  zh_ = zookeeper_init(hosts.c_str(), mywatcher, timeout * 1000, 0, this, 0);
  if (!zh_) {
    perror("");
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(
            "failed to initialize zk: " + hosts)
        << core::common::exception::error_api_func("zookeeper_init")
        << core::common::exception::error_errno(errno));
  }

  if (!wait_until_connected(timeout)) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("cannot connect zk in "
            + jubatus::util::lang::lexical_cast<std::string, int>(timeout)
            + " sec: " + hosts));
  }

  LOG(INFO) << "connected to zk: " << get_connected_host_and_port();
}

zk::~zk() {
  force_close();
  if (logfilep_) {
    fclose(logfilep_);
    logfilep_ = NULL;
  }
}

/**
 * Wait until ZooKeeper session to be established (ZOO_CONNECTED_STATE), for
 * up to `timeout` seconds.  Returns true when succeed, false when timed out.
 */
bool zk::wait_until_connected(int timeout) {
  scoped_lock lk(m_);
  int retry = timeout;
  while (zoo_state(zh_) != ZOO_CONNECTED_STATE) {
    if (retry <= 0) {
      return false;
    } else {
      retry--;
      ::sleep(1);  // 1 sec
    }
  }
  return true;
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
      LOG(ERROR) << "failed to create ZooKeeper ephemeral node: "
                 << path << ": " << zerror(rc) << " (" << rc << ")";
      return false;
    }
  } else {
    if (rc != ZOK && rc != ZNODEEXISTS) {
      LOG(ERROR) << "failed to create ZooKeeper node: "
                 << path << ": " << zerror(rc) << " (" << rc << ")";
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
    LOG(ERROR) << "failed to write to ZooKeeper node: "
               << path << ": " << zerror(rc) << " (" << rc << ")";
    return false;
  }
  DLOG(INFO) << __func__ << " " << path << " - " << payload;
  return true;
}

// "/some/path" => "/some/path0000012"
bool zk::create_seq(const string& path, string& seqfile) {
  scoped_lock lk(m_);
  std::vector<char> path_buffer(path.size() + 16);
  int rc = zoo_create(zh_, path.c_str(), NULL, 0, &ZOO_OPEN_ACL_UNSAFE,
                      ZOO_EPHEMERAL | ZOO_SEQUENCE, &path_buffer[0],
                      path_buffer.size());
  if (rc != ZOK) {
    seqfile.clear();
    LOG(ERROR) << "failed to create ZooKeeper sequencial node: "
               << path << ": " << zerror(rc) << " (" << rc << ")";
    return false;
  } else {
    seqfile.assign(&path_buffer[0]);
    DLOG(INFO) << __func__ << " " << seqfile;
    return true;
  }
}

bool zk::create_id(const string& path, uint32_t prefix, uint64_t& res) {
  scoped_lock lk(m_);
  struct Stat st;
  int rc = zoo_set2(zh_, path.c_str(), "dummy", 6, -1, &st);

  if (rc != ZOK) {
    LOG(ERROR) << "failed to increment version of ZooKeeper node: "
               << path << ": " << zerror(rc) << " (" << rc << ")";
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
    LOG(ERROR) << "failed to remove ZooKeeper node: "
               << path << ": " << zerror(rc) << " (" << rc << ")";
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
  jubatus::util::lang::function<void(int, int, string)>* fp =
      static_cast<jubatus::util::lang::function<void(int, int, string)>*>(
          watcherCtx);
  try {
    (*fp)(type, state, string(path));
  } catch(const std::exception& e) {
    LOG(WARNING) << "exception thrown from zk watcher callback: " << e.what();
  } catch (...) {
    LOG(WARNING) << "unknown exception thrown from zk watcher callback";
  }
  delete fp;
}

bool zk::bind_watcher(
    const string& path,
    jubatus::util::lang::function<void(int, int, string)>& f) {
  jubatus::util::lang::function<void(int, int, string)>* fp =
    new jubatus::util::lang::function<void(int, int, string)>(f);
  int rc = zoo_wexists(zh_, path.c_str(), my_znode_watcher, fp, NULL);
  return rc == ZOK;
}

bool zk::bind_child_watcher(
    const string& path,
    const jubatus::util::lang::function<void(int, int, string)>& f) {
  jubatus::util::lang::function<void(int, int, string)>* fp =
    new jubatus::util::lang::function<void(int, int, string)>(f);
  int rc = zoo_wget_children(zh_, path.c_str(), my_znode_watcher, fp, NULL);
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
    jubatus::util::lang::function<void(string)>* fp =
        static_cast<jubatus::util::lang::function<void(string)>*>(watcherCtx);
    try {
      (*fp)(string(path));
    } catch(const std::exception& e) {
      LOG(WARNING) << "exception thrown from zk watcher callback: " << e.what();
    } catch (...) {
      LOG(WARNING) << "unknown exception thrown from zk watcher callback";
    }
    delete fp;
  } else if (type == ZOO_SESSION_EVENT) {
    /* - ignore
       the ZK server may down, and Jubatus will retry other ZK.
       Jubatus handles SESSION_EVENT in `mywatcher` correctly,
       so this delete_watcher does not need to anything for SESSION_EVENT.
     */
    return;  // don't re-register
  } else if (type == ZOO_CHANGED_EVENT) {
    LOG(INFO) << "ZK delete watcher got CHANGED event: " << path;
  } else if (type == ZOO_CREATED_EVENT) {
    LOG(WARNING) << "ZK delete watcher got CREATED event, "
                 << "something wrong: " << path;
  } else if (type == ZOO_CHILD_EVENT) {
    LOG(WARNING) << "ZK delete watcher got CHILD event, "
                 << "something wrong: " << path;
  } else {
    DLOG(FATAL)
        << "unknown ZK event happen in path: ["
        << path << "] type:["
        << type << "] state:["
        << state << "]";
  }

  int rc = zoo_wexists(zh, path, my_znode_delete_watcher, watcherCtx, NULL);
  if (rc != ZOK) {
    LOG(WARNING) << "failed to reregister ZK delete watcher: "
                 << path << ": " << zerror(rc) << " (" << rc << ")";
  }
}

bool zk::bind_delete_watcher(
    const string& path,
    jubatus::util::lang::function<void(string)>& f) {
  jubatus::util::lang::function<void(string)>* fp =
    new jubatus::util::lang::function< void(string)>(f);
  int rc = zoo_wexists(zh_, path.c_str(), my_znode_delete_watcher, fp, NULL);
  return rc == ZOK;
}

bool zk::list(const string& path, vector<string>& out) {
  scoped_lock lk(m_);
  return list_(path, out);
}

namespace {

class string_vector_holder : util::lang::noncopyable {
 public:
  string_vector_holder()
    : v_()  // set null
  {}

  ~string_vector_holder() {
    release();
  }

  int zoo_get_children(zhandle_t* zh, const char* path, int watch) {
    // releasing memory here may break strong exception safety,
    // but such case is user's fault.
    release();
    return ::zoo_get_children(zh, path, watch, &v_);
  }

  int32_t size() const {
    return v_.count;
  }

  const char* operator[](int32_t n) const {
    return v_.data[n];
  }

  void release() {
    if (v_.data != 0) {
      deallocate_String_vector(&v_);
      v_.data = 0;
      v_.count = 0;
    }
  }

 private:
  String_vector v_;
};
}  // namespcae

bool zk::list_(const string& path, vector<string>& out) {
  string_vector_holder sv;
  int rc = sv.zoo_get_children(zh_, path.c_str(), 0);
  switch (rc) {
  case ZOK:
    for (int32_t i = 0; i < sv.size(); ++i) {
      out.push_back(sv[i]);  // full path => #{path}/#{s.data[i]}
    }
    std::sort(out.begin(), out.end());
    return true;

  case ZNONODE:
    return true;

  default:
    LOG(ERROR) << "failed to get all child nodes of ZooKeeper node: "
               << path << ": " << zerror(rc) << " (" << rc << ")";
    return false;
  }
}

bool zk::hd_list(const string& path, string& out) {
  string_vector_holder sv;
  scoped_lock lk(m_);
  int rc = sv.zoo_get_children(zh_, path.c_str(), 0);
  if (rc == ZOK) {
    if (sv.size() > 0) {
      out = sv[0];
    }
    return true;
  }
  LOG(ERROR) << "failed to get a child node of ZooKeeper node: "
             << path << ": " << zerror(rc) << " (" << rc << ")";
  return false;
}

bool zk::read(const string& path, string& out) {
  scoped_lock lk(m_);

  Stat stat;
  int rc = zoo_exists(zh_, path.c_str(), 0, &stat);
  if (rc != ZOK) {
    LOG(ERROR) << "failed to get info of ZooKeeper node: "
               << path << ": " << zerror(rc) << " (" << rc << ")";
    return false;
  }

  for (;;) {
    int buflen = stat.dataLength;

    if (buflen < 0) {
      LOG(ERROR) << "ZooKeeper node length is invalid (" << buflen << "): "
                 << path;
      return false;
    } else if (buflen == 0) {
      out.clear();
      return true;
    }

    std::vector<char> buf(buflen);
    rc = zoo_get(zh_, path.c_str(), 0, &buf[0], &buflen, &stat);
    if (rc != ZOK) {
      LOG(ERROR) << "failed to read from ZooKeeper node: "
                 << path << ": " << zerror(rc) << " (" << rc << ")";
      return false;
    }
    JUBATUS_ASSERT_GE(buflen, 0, "NULL data should have been checked");
    JUBATUS_ASSERT_LE(buflen, static_cast<int>(buf.size()), "");

    if (buflen == stat.dataLength) {
      out.assign(&buf[0], buflen);
      return true;
    }

    LOG(INFO) << "failed to read from ZooKeeper node; "
              << "length changed while reading, retrying: "
              << path << ": " << zerror(rc) << " (" << rc << ")";
  }
}

void zk::push_cleanup(const jubatus::util::lang::function<void()>& f) {
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

const std::string zk::get_connected_host_and_port() const {
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

  // TODO(@rimms): add this code to common::util
  if (sock_addr.sa_family == AF_INET) {  // Only IPv4
    struct sockaddr_in* sock_addr_in = (struct sockaddr_in *) &sock_addr;
    char host[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sock_addr_in->sin_addr), host, sizeof(host));
    ret << host << ":" << ntohs(sock_addr_in->sin_port);
  }

  return ret.str();
}

bool zkmutex::lock() {
  jubatus::util::concurrent::scoped_lock lk(m_);
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
  jubatus::util::concurrent::scoped_lock lk(m_);
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
  jubatus::util::concurrent::scoped_lock lk(m_);
  if (has_lock_) {
    return zk_.remove(seqfile_);
  }
  return true;
}

bool zkmutex::rlock() {
  jubatus::util::concurrent::scoped_lock lk(m_);
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
  jubatus::util::concurrent::scoped_lock lk(m_);
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
    if (jubatus::util::data::string::starts_with(list[i], string("wlock_"))) {
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
  jubatus::util::concurrent::scoped_lock lk(m_);
  if (has_rlock_) {
    return zk_.remove(seqfile_);
  }
  return true;
}

void mywatcher(zhandle_t* zh, int type, int state, const char* path, void* p) {
  zk* zk_ = static_cast<zk*>(p);

  LOG(INFO) << "got ZooKeeper event: "
            << "type " << zk_type2str(type) << "(" << type << "), "
            << "state " << zk_state2str(state) << "(" << state << ")";

  // ZOO_* cannot use switch because it const int.
  if (type == ZOO_CREATED_EVENT) {
  } else if (type == ZOO_DELETED_EVENT) {
  } else if (type == ZOO_CHANGED_EVENT) {
  } else if (type == ZOO_CHILD_EVENT) {
  } else if (type == ZOO_SESSION_EVENT) {
    if (state == ZOO_CONNECTED_STATE) {
      LOG(INFO) << "ZooKeeper session established, negotiated timeout "
                << zoo_recv_timeout(zh) << " ms";
    } else if (state == ZOO_CONNECTING_STATE) {
      LOG(WARNING) << "ZooKeeper session lost";

      // Wait to expect the ZooKeeper master failover.
      // `zoo_recv_timeout` returns the timeout negotiated between the
      // last-connected ZooKeeper cluster and the client.
      if (zk_->wait_until_connected(::ceil(zoo_recv_timeout(zh) / 1000.f))) {
        LOG(INFO) << "ZooKeeper session recovered before timeout";
      } else {
        int final_state = zoo_state(zh);
        LOG(ERROR) << "ZooKeeper session recovery timed out: "
                   << "state " << zk_state2str(final_state)
                   << "(" << final_state << ")";
        zk_->run_cleanup();
      }
    } else if (state == ZOO_EXPIRED_SESSION_STATE) {
      LOG(ERROR) << "ZooKeeper session expired";
      zk_->run_cleanup();
    } else if (state == ZOO_ASSOCIATING_STATE) {
    } else if (state == ZOO_AUTH_FAILED_STATE) {
    } else {
      LOG(ERROR) << "unknown ZooKeeper session state: " << state;
    }
  } else if (type == ZOO_NOTWATCHING_EVENT) {
  } else {
    LOG(ERROR) << "unknown ZooKeeper event: "
               << "type " << type << ", state " << state;
  }
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
