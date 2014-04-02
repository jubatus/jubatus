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
#include <vector>
#include <gtest/gtest.h>
#include "../common/exception.hpp"
#include "lru_unlearner.hpp"
#include "test_util.hpp"

namespace jubatus {
namespace core {
namespace unlearner {

TEST(lru_unlearner, max_size_must_be_positive) {
  lru_unlearner::config config;
  config.max_size = -1;
  EXPECT_THROW(lru_unlearner unlearner(config), common::config_exception);
  config.max_size = 0;
  EXPECT_THROW(lru_unlearner unlearner(config), common::config_exception);
}

TEST(lru_unlearner, trivial) {
  lru_unlearner::config config;
  config.max_size = 3;
  lru_unlearner unlearner(config);

  mock_callback callback;
  unlearner.set_callback(callback);

  std::vector<std::string> touch_sequence;
  touch_sequence.push_back("id1");
  touch_sequence.push_back("id2");
  touch_sequence.push_back("id3");
  touch_sequence.push_back("id2");
  touch_sequence.push_back("id1");

  for (size_t i = 0; i < touch_sequence.size(); ++i) {
    unlearner.touch(touch_sequence[i]);
    EXPECT_EQ("", callback.unlearned_id());
    EXPECT_TRUE(unlearner.exists_in_memory(touch_sequence[i]));
  }

  unlearner.touch("id4");
  EXPECT_EQ("id3", callback.unlearned_id());
  EXPECT_FALSE(unlearner.exists_in_memory(callback.unlearned_id()));
}

}  // namespace unlearner
}  // namespace core
}  // namespace jubatus
