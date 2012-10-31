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

#include <vector>
#include <pficommon/lang/function.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/concurrent/mutex.h>
#include <pficommon/concurrent/lock.h>

#include <stdint.h>

namespace jubatus {
namespace common {

// TODO: write lock_service mock and test them all?
class lock_service {
public:
  // timeout [sec]
  lock_service() {}
  virtual ~lock_service() {}

  virtual void force_close() = 0;
  virtual void create(const std::string& path, const std::string& payload = "", bool ephemeral = false) = 0;
  virtual void remove(const std::string& path) = 0;
  virtual bool exists(const std::string& path) = 0;

  virtual bool bind_watcher(const std::string& path, pfi::lang::function<void(int,int,std::string)>&) = 0;

  // ephemeral only
  virtual void create_seq(const std::string& path, std::string&) = 0;
  virtual uint64_t create_id(const std::string& path, uint32_t prefix = 0) = 0;

  virtual void list(const std::string& path, std::vector<std::string>& out) = 0;
  virtual void hd_list(const std::string& path, std::string& out) = 0;

  // reads data (should be smaller than 1024B)
  virtual bool read(const std::string& path, std::string& out) = 0;

  virtual void push_cleanup(pfi::lang::function<void()>& f) = 0;
  virtual void run_cleanup() = 0;

  virtual const std::string& get_hosts() const = 0;
  virtual const std::string type() const = 0;
};

class try_lockable : public pfi::concurrent::lockable {
public:
  virtual bool try_lock() = 0;
};

class lock_service_mutex : public try_lockable {
public:
  lock_service_mutex(lock_service& ls, const std::string& path);
  virtual ~lock_service_mutex() {
    delete impl_;
  }

  bool lock();
  bool try_lock();
  bool unlock();

protected:
  try_lockable* impl_;
  std::string path_;
};

lock_service* create_lock_service(const std::string&,
    const std::string& hosts, const int timeout, const std::string& log = "/tmp/zklog");

} // common
} // jubatus
