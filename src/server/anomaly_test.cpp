// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include "anomaly_client.hpp"
#include <vector>
#include <string>
#include "test_util.hpp"

using namespace std;
using namespace jubatus;

static const int PORT = 65436;

namespace {

  class anomaly_test : public ::testing::Test {
  protected:
    pid_t child_;

    anomaly_test(){
      child_ = fork_process("anomaly", PORT);
    };
    virtual ~anomaly_test(){
      kill_process(child_);
    };
    virtual void restart_process(){

      kill_process(this->child_);
      this->child_ = fork_process("anomaly");
    };
  };


TEST_F(anomaly_test, small){

  client::anomaly c("localhost", PORT, 10);

  // FIXME: write tests
  //  c.get_config("");
}
}
