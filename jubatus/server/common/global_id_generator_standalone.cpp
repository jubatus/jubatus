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

#include <cassert>
#include <string>

#ifndef ATOMIC_I8_SUPPORT
#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/mutex.h>
#endif

#include "jubatus/core/common/exception.hpp"
#include "global_id_generator_base.hpp"
#include "global_id_generator_standalone.hpp"

namespace jubatus {
namespace server {
namespace common {

struct global_id_generator_standalone::impl {
  impl() : counter(0) {}

  uint64_t counter;
#ifndef ATOMIC_I8_SUPPORT
  pfi::concurrent::mutex counter_mutex;
#endif
};

global_id_generator_standalone::global_id_generator_standalone()
    : pimpl_(new impl) {
}

global_id_generator_standalone::~global_id_generator_standalone() {
}

uint64_t global_id_generator_standalone::generate() {
#ifdef ATOMIC_I8_SUPPORT
  return __sync_fetch_and_add(&pimpl_->counter, 1);
#else
  pfi::concurrent::scoped_lock lk(pimpl_->counter_mutex);
  return pimpl_->counter++;
#endif
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
