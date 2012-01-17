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
#include "regression_client.hpp"
#include <vector>
#include <string>
#include "../fv_converter/datum.hpp"
#include "../fv_converter/converter_config.hpp"

#include "test_util.hpp"

using namespace std;
using namespace jubatus;

static const int PORT = 65434;

namespace {

  class regression_test : public ::testing::Test {
  protected:
    pid_t child_;

    regression_test(){
      child_ = fork_process("regression", PORT);
    };
    virtual ~regression_test(){
      kill_process(child_);
    };
    virtual void restart_process(){

      kill_process(this->child_);
      this->child_ = fork_process("regression");
    };
  };


TEST_F(regression_test, small) {

  client::regression c("localhost", PORT, 10);
  
  cout << "set_config" << endl;
  config_data conf;
  conf.method = "PA";
  num_rule r = {"*", "num"};
  conf.config.num_rules.push_back(r);
  c.set_config("test", conf);

  cout << "train" << endl;
  vector<pair<float, datum> > data;
  datum d;
  d.nv.push_back(make_pair("f1", 1.0));
  data.push_back(make_pair(10, d));
  c.train("test", data);

  cout << "get_status" << endl;
  map<string,map<string,string> > status = c.get_status("test", 0);
  EXPECT_EQ(status.size(), 1u);
  for(map<string,map<string,string> >::const_iterator it = status.begin();
      it != status.end(); ++it){
    EXPECT_GE(it->second.size(), 8u);
  }
  cout << "estimate" << endl;
  vector<datum> test;
  test.push_back(d);
  vector<float> res = c.estimate("test", test);
  cout << res.size() << endl;
}

}
