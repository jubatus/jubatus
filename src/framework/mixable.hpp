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

#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/concurrent/rwmutex.h>
#include <msgpack.hpp>

#include "../common/exception.hpp"
#include "../common/shared_ptr.hpp"
#include "../common/mprpc/byte_buffer.hpp"

namespace jubatus {
namespace framework {


class mixable0 {
public:
  mixable0() {}
  virtual ~mixable0() {}

  virtual void save(std::ostream & ofs) = 0;
  virtual void load(std::istream & ifs) = 0;
  virtual void clear() = 0;
};

// bundler interface
class model_mixer_base {
public:
  virtual ~model_mixer_base() {}
};

class model_bundler;

class mixable_holder {
public:
  mixable_holder(pfi::lang::shared_ptr<model_bundler> b)
    : bundler_(b)
  {}
  virtual ~mixable_holder() {}

  model_bundler* get_bundler() const {
    return bundler_.get();
  }

  pfi::concurrent::rw_mutex& rw_mutex() {
    return rw_mutex_;
  }

protected:
  pfi::concurrent::rw_mutex rw_mutex_;
  pfi::lang::shared_ptr<model_bundler> bundler_;
};




} // framework
} // jubatus

#include "diff_mixable.hpp"
#include "model_bundler.hpp"
