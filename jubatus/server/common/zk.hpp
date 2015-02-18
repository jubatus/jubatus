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

#ifndef JUBATUS_SERVER_COMMON_ZK_HPP_
#define JUBATUS_SERVER_COMMON_ZK_HPP_

#include <string>
#include <vector>

#include "jubatus/util/lang/function.h"
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/util/concurrent/mutex.h"
#include "jubatus/util/concurrent/lock.h"
#include "jubatus/util/concurrent/threading_model.h"

#include "lock_service.hpp"
#include "logger/logger.hpp"
#include ZOOKEEPER_HEADER

namespace jubatus {
namespace server {
namespace common {
// TODO(kashihara): write zk mock and test them all?

class zk : public lock_service {
 public:
  // timeout [sec]
  zk(
      const std::string& hosts,
      int timeout = 10,
      const std::string& logfile = "");

  virtual ~zk();

  bool wait_until_connected(int timeout);
  void force_close();
  bool create(
      const std::string& path,
      const std::string& payload = "",
      bool ephemeral = false);
  bool set(const std::string& path, const std::string& payload = "");
  bool remove(const std::string& path);
  bool exists(const std::string& path);

  bool bind_watcher(
      const std::string& path,
      jubatus::util::lang::function<void(int, int, std::string)>&);

  bool bind_child_watcher(
      const std::string& path,
      const jubatus::util::lang::function<void(int, int, std::string)>&);

  bool bind_delete_watcher(
      const std::string& path,
      jubatus::util::lang::function<void(std::string)>&);

  // ephemeral only
  bool create_seq(const std::string& path, std::string&);
  bool create_id(const std::string& path, uint32_t prefix, uint64_t& res);

  // returns unsorted list
  bool list(const std::string& path, std::vector<std::string>& out);
  bool hd_list(const std::string& path, std::string& out);

  // reads data (should be smaller than 1024B)
  bool read(const std::string& path, std::string& out);

  void push_cleanup(const jubatus::util::lang::function<void()>& f);
  void run_cleanup();

  const std::string& get_hosts() const;
  const std::string type() const;

  const std::string get_connected_host_and_port() const;

 protected:
  bool list_(const std::string& path, std::vector<std::string>& out);

  zhandle_t* zh_;
  clientid_t* cid_;
  const std::string hosts_;

  jubatus::util::concurrent::mutex m_;
  std::vector<jubatus::util::lang::function<void()> > cleanups_;

  FILE* logfilep_;
};

// TODO(kashihara): write zk mock and test them all?
class zkmutex : public try_lockable {
 public:
  zkmutex(lock_service& ls, const std::string& path)
      : zk_(ls),
        path_(path),
        has_lock_(false),
        has_rlock_(false) {
  }
  virtual ~zkmutex() {
    this->unlock();
    this->unlock_r();
  }

  bool lock();
  bool try_lock();
  bool unlock();
  bool rlock();
  bool try_rlock();
  bool unlock_r();

 private:
  lock_service& zk_;
  std::string path_;
  std::string seqfile_;
  bool has_lock_;
  bool has_rlock_;
  jubatus::util::concurrent::mutex m_;
};

void mywatcher(zhandle_t*, int, int, const char*, void*);

}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_ZK_HPP_
