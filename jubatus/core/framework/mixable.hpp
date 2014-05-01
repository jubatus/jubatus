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

#ifndef JUBATUS_CORE_FRAMEWORK_MIXABLE_HPP_
#define JUBATUS_CORE_FRAMEWORK_MIXABLE_HPP_

#include <iostream>
#include <string>
#include <vector>

#include <msgpack.hpp>
#include "jubatus/util/concurrent/lock.h"
#include "jubatus/util/lang/shared_ptr.h"
#include "../common/version.hpp"
#include "../common/exception.hpp"
#include "../common/byte_buffer.hpp"
#include "../common/assert.hpp"

// TODO(suma): Rename new_mixable.hpp to mixable.hpp when
#include "linear_mixable.hpp"
#include "push_mixable.hpp"
#include "new_mixable.hpp"
#include "model.hpp"

namespace jubatus {
namespace core {
namespace framework {

// TODO(suma): separate other header
class mixable_holder {
 public:
  typedef std::vector<jubatus::util::lang::shared_ptr<mixable> > mixable_list;

  mixable_holder() {
  }

  virtual ~mixable_holder() {
  }

  void register_mixable(jubatus::util::lang::shared_ptr<mixable> m) {
    mixables_.push_back(m);
  }

  std::vector<storage::version> get_versions() {
    std::vector<storage::version> ret;
    for (size_t i = 0; i < mixables_.size(); ++i) {
      ret.push_back(mixables_[i]->get_version());
    }
    return ret;
  }

 protected:
  std::vector<jubatus::util::lang::shared_ptr<mixable> > mixables_;
};


}  // namespace framework
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FRAMEWORK_MIXABLE_HPP_
