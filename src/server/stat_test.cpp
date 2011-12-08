// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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


#include "gtest/gtest.h"
#include "stat_client.hpp"
#include <vector>
#include <string>
#include "test_util.hpp"

using namespace std;
using namespace jubatus;

namespace {

  class stat_test : public ::testing::Test {
  protected:
    pid_t child_;

    stat_test(){
      child_ = fork_process("stat", 9197);
    };
    virtual ~stat_test(){
      kill_process(child_);
    };
    virtual void restart_process(){

      kill_process(this->child_);
      this->child_ = fork_process("stat");
    };
  };


TEST_F(stat_test, small) {

  client::stat c("localhost", 9197, 10);
  c.push("", "hoge", 12);
  ASSERT_DOUBLE_EQ(12.0, c.sum("", "hoge", 0));
  //  ASSERT_DOUBLE_EQ(12.0, c.stddev("", "hoge", 0));
}
}
