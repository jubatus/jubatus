// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "../stat/stat.hpp"
#include "stat.hpp"
#include "test_util.hpp"

namespace jubatus {
namespace core {
namespace driver {

class stat_test : public ::testing::Test {
 protected:
  virtual void SetUp() {
    const int32_t window_size = 128;
    stat_.reset(new core::driver::stat(new core::stat::stat(window_size)));
  }

  virtual void TearDown() {
    stat_.reset();
  }

  pfi::lang::shared_ptr<core::driver::stat> stat_;
};

TEST_F(stat_test, small) {
  stat_->push("hoge", 12);
  ASSERT_DOUBLE_EQ(12.0, stat_->sum("hoge"));

  // TODO(kuenishi): add more tests
  ASSERT_DOUBLE_EQ(.0, stat_->stddev("hoge"));
  ASSERT_DOUBLE_EQ(12.0, stat_->max("hoge"));
  ASSERT_DOUBLE_EQ(12.0, stat_->min("hoge"));

  ASSERT_DOUBLE_EQ(0., stat_->entropy());

  std::string save_data;
  save_model(stat_->get_mixable_holder(), save_data);
  load_model(stat_->get_mixable_holder(), save_data);
}

}  // driver namespace
}  // core namespace
}  // jubatus namespace
