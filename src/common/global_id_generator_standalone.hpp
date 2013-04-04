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

#ifndef JUBATUS_COMMON_GLOBAL_ID_GENERATOR_STANDALONE_HPP_
#define JUBATUS_COMMON_GLOBAL_ID_GENERATOR_STANDALONE_HPP_

#include <stdint.h>
#include <string>

#ifndef ATOMIC_I8_SUPPORT
#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/mutex.h>
#endif

#include "global_id_generator_base.hpp"

namespace jubatus {
namespace common {

class global_id_generator_standalone: public global_id_generator_base {
 public:
  global_id_generator_standalone();
  virtual ~global_id_generator_standalone();

  virtual uint64_t generate();

 private:
  uint64_t counter_;
#ifndef ATOMIC_I8_SUPPORT
  pfi::concurrent::mutex counter_mutex_;
#endif
};

}  // namespace common
}  // namespace jubatus

#endif  // JUBATUS_COMMON_GLOBAL_ID_GENERATOR_HPP_
