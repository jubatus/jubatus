// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_UNLEARNER_TEST_UTIL_HPP_
#define JUBATUS_CORE_UNLEARNER_TEST_UTIL_HPP_

#include <string>
#include "jubatus/util/lang/shared_ptr.h"

namespace jubatus {
namespace core {
namespace unlearner {

// Mock class of unlearning callback. It just holds the argument of last call.
class mock_callback {
 public:
  mock_callback()
      : unlearned_id_(new std::string) {
  }

  void operator()(const std::string& id) {
    *unlearned_id_ = id;
  }

  std::string unlearned_id() const {
    return *unlearned_id_;
  }

 private:
  jubatus::util::lang::shared_ptr<std::string> unlearned_id_;
};

}  // namespace unlearner
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_UNLEARNER_TEST_UTIL_HPP_
