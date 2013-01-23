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
#include <vector>

#include <glog/logging.h>

#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>
#include <pficommon/math/random.h>

#include "../common/lock_service.hpp"
#include "../common/cht.hpp"
#include "../common/shared_ptr.hpp"
#include "../common/exception.hpp"

#include "server_util.hpp"

namespace jubatus {
namespace framework {

class no_worker : public jubatus::exception::runtime_error {
 public:
  no_worker(const std::string& name)
      : runtime_error("no server found: " + name) {
  }
};

class keeper_common {
 public:
  keeper_common(const keeper_argv &a);
  virtual ~keeper_common();

 protected:
  void get_members_(const std::string& name,
                    std::vector<std::pair<std::string, int> >& ret);
  void get_members_from_cht_(const std::string& name, const std::string& id,
                             std::vector<std::pair<std::string, int> >& ret,
                             size_t n);

  keeper_argv a_;
  pfi::math::random::mtrand rng_;
  pfi::concurrent::mutex mutex_;
  common::cshared_ptr<common::lock_service> zk_;
};

}  // framework
}  // jubatus

