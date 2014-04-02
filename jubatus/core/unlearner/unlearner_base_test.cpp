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

#include <string>
#include <gtest/gtest.h>
#include "test_util.hpp"
#include "unlearner_base.hpp"

namespace jubatus {
namespace core {
namespace unlearner {
namespace {

class mock_unlearner : public unlearner_base {
 public:
  void touch(const std::string& id) {
  }

  void call_unlearn(const std::string& id) const {
    unlearn(id);
  }

  bool exists_in_memory(const std::string& id) const {
    // mock unlearner does not remember anything
    return false;
  }

  void save(std::ostream& os) const {
  }

  void load(std::istream& is) {
  }
};

}  // namespace

TEST(unlearner_base, unlearn) {
  std::string expect("my_id");
  mock_callback callback;
  mock_unlearner unlearner;
  unlearner.set_callback(callback);
  unlearner.call_unlearn(expect);

  EXPECT_EQ(expect, callback.unlearned_id());
}

}  // namespace unlearner
}  // namespace core
}  // namespace jubatus
