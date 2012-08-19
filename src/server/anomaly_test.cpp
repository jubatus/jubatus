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
static const std::string NAME = "";

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


void make_default_config(jubatus::config_data& c)
{
  c.config["anomaly:name"] = "lof";
  c.config["lof:neighbor_num"] = "10";
  c.config["lof:reverse_nn_num"] = "30";
  c.config["name"] = "euclid_lsh";
  c.config["lsh_num"] = "8";
  c.config["table_num"] = "16";
  c.config["probe_num"] = "64";
  c.config["bin_width"] = "10";
  
  jubatus::fv_converter::converter_config config;
  jubatus::fv_converter::num_rule rule = { "*", "num" };
  config.num_rules.push_back(rule);
  c.converter = config_to_string(config);
}

TEST_F(anomaly_test, small){

  client::anomaly c("localhost", PORT, 10);

  {
    jubatus::config_data config, config2;
    make_default_config(config);

    c.set_config(NAME, config);
    config2 = c.get_config(NAME);
    ASSERT_EQ(config.config, config2.config);
    ASSERT_EQ(config.converter, config2.converter);
  }
  {
    datum d;
    d.nv.push_back(make_pair("f1", 1.0));

    c.add(NAME, d); // is it good to be inf?
    std::pair<std::string, float> w = c.add(NAME, d);
    float v = c.calc_score(NAME, d);
    ASSERT_DOUBLE_EQ(w.second, v);
  }
  {
    std::vector<std::string> rows = c.get_all_rows(NAME);
    ASSERT_EQ(2u, rows.size());
  }
  c.save(NAME, "id");
  c.clear(NAME);
  {
    std::vector<std::string> rows = c.get_all_rows(NAME);
    ASSERT_EQ(0u, rows.size());
  }
  c.load(NAME, "id");
  {
    std::vector<std::string> rows = c.get_all_rows(NAME);
    ASSERT_EQ(2u, rows.size());
  }
  { c.get_status(NAME);
  }
}
}
