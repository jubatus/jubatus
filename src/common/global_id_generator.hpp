// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <stdint.h>

#include "lock_service.hpp"
#include "shared_ptr.hpp"

#ifndef ATOMIC_I8_SUPPORT
#include <pficommon/concurrent/mutex.h>
#endif

namespace jubatus { namespace common {

class global_id_generator
{
public:

  global_id_generator(bool);
  ~global_id_generator();

  uint64_t generate();

  void set_ls(cshared_ptr<lock_service>&, const std::string&);

private:
  global_id_generator();
  bool is_standalone_;
  uint64_t counter_;

  std::string path_;
  cshared_ptr<lock_service> ls_;

#ifndef ATOMIC_I8_SUPPORT
  pfi::concurrent::mutex counter_mutex_;
#endif
};

}}
