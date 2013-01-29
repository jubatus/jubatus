// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "global_id_generator.hpp"
#include <cassert>
#include <string>

#ifndef ATOMIC_I8_SUPPORT
#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/mutex.h>
#endif

#include "exception.hpp"

namespace jubatus {
namespace common {

class global_id_generator_impl {
 public:
  global_id_generator_impl()
      : is_standalone_(true),
        counter_(0) {
  }

  explicit global_id_generator_impl(bool is_standalone)
      : is_standalone_(is_standalone),
        counter_(0) {
  }

  uint64_t generate() {
    if (is_standalone_) {
#ifdef ATOMIC_I8_SUPPORT
      return __sync_fetch_and_add(&counter_, 1);
#else
      pfi::concurrent::scoped_lock lk(counter_mutex_);
      return counter_++;
#endif
    } else {
#ifdef HAVE_ZOOKEEPER_H
      uint64_t res;
      if (ls_->create_id(path_, 0, res)) {
        return res;
      } else {
        throw JUBATUS_EXCEPTION(
            jubatus::exception::runtime_error("Failed to create id"));
      }
#else
      // never reaches here
      assert(is_standalone_);
      return 0;  // dummy to remove warning
#endif
    }
  }

  void set_ls(cshared_ptr<lock_service>& ls, const std::string& path_prefix) {
#ifdef HAVE_ZOOKEEPER_H
    if (!is_standalone_) {
      path_ = path_prefix + "/id_generator";
      ls_ = ls;
      if (!ls_->create(path_)) {
      throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
            "Failed to create global id generator")
          << jubatus::exception::error_api_func("lock_service::create")
          << jubatus::exception::error_message(path_));
      }
    }
#endif
  }

 private:
  bool is_standalone_;
  uint64_t counter_;
  std::string path_;
  cshared_ptr<lock_service> ls_;

#ifndef ATOMIC_I8_SUPPORT
  pfi::concurrent::mutex counter_mutex_;
#endif
};

global_id_generator::global_id_generator()
    : pimpl_(new global_id_generator_impl()) {
}

global_id_generator::global_id_generator(bool is_standalone)
    : pimpl_(new global_id_generator_impl(is_standalone)) {
}

global_id_generator::~global_id_generator() {
}

uint64_t global_id_generator::generate() {
  return pimpl_->generate();
}

void global_id_generator::set_ls(cshared_ptr<lock_service>& ls,
    const std::string& path_prefix) {
  pimpl_->set_ls(ls, path_prefix);
}

}  // namespace common
}  // namespace jubatus
