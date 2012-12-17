// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

std::string make_simple_config(const string& method) {
  using namespace pfi::text::json;

  json js(new json_object());
  js["method"] = json(new json_string(method));

  json anomaly_config(new json_object());
  anomaly_config["method"] = json(new json_string("euclid_lsh"));
  anomaly_config["neighbor_num"] = json(new json_integer(100));
  anomaly_config["reverse_nn_num"] = json(new json_integer(30));

  json nn_config(new json_object());
  nn_config["lsh_num"] = json(new json_integer(8));
  nn_config["table_num"] = json(new json_integer(16));
  nn_config["probe_num"] = json(new json_integer(64));
  nn_config["bin_width"] = json(new json_integer(10));
  nn_config["seed"] = json(new json_integer(1234));
  nn_config["retain_projection"] = json(new json_bool(true));

  anomaly_config["nearest_neighbor"] = nn_config;
  js["anomaly"] = anomaly_config;

  jubatus::fv_converter::converter_config config;
  jubatus::fv_converter::num_rule rule = { "*", "num" };
  config.num_rules.push_back(rule);
  std::stringstream conv;
  conv << config_to_string(config);
  json jsc;
  conv >> jsc;
  js["converter"] = jsc;

  std::stringstream ret;
  ret << pretty(js);

  return ret.str();
}

TEST_F(anomaly_test, small){

  client::anomaly c("localhost", PORT, 10);

  {
    string config = make_simple_config("lof");
    string config2;

    c.set_config(NAME, config);
    config2 = c.get_config(NAME);
    ASSERT_EQ(config, config2);
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
