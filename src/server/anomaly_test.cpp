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

#include <gtest/gtest.h>

#include <string>
#include <utility>
#include <vector>

#include "anomaly_client.hpp"
#include "test_util.hpp"
#include "../anomaly/lof_storage.hpp"
#include "../recommender/euclid_lsh.hpp"

using std::string;
using std::make_pair;
using jubatus::datum;
using jubatus::recommender::euclid_lsh;

static const int PORT = 65436;
static const std::string NAME = "";

namespace {

class anomaly_test : public ::testing::Test {
 protected:
  pid_t child_;

  anomaly_test() {
    child_ = fork_process("anomaly",
                          PORT,
                          "./test_input/config.anomaly.json");
  }
  virtual ~anomaly_test() {
    kill_process(child_);
  }
  virtual void restart_process() {
    kill_process(this->child_);
    this->child_ = fork_process("anomaly",
                                PORT,
                                "./test_input/config.anomaly.json");
  }
};

std::string make_simple_config(const string& method) {
  using pfi::text::json::json;
  using pfi::text::json::json_object;
  using pfi::text::json::json_string;
  using pfi::text::json::json_integer;
  using pfi::text::json::to_json;

  json js(new json_object());
  js["method"] = json(new json_string(method));

  json anomaly_config(new json_object());
  anomaly_config["method"] = json(new json_string("euclid_lsh"));
  anomaly_config["nearest_neighbor_num"] = json(new json_integer(100));
  anomaly_config["reverse_nearest_neighbor_num"] = json(new json_integer(30));

  euclid_lsh::config euclid_conf;
  euclid_conf.lsh_num = 8;
  euclid_conf.table_num = 8;
  euclid_conf.probe_num = 8;
  euclid_conf.bin_width = 8.2;  // float
  euclid_conf.seed = 1234;
  euclid_conf.retain_projection = true;

  anomaly_config["parameter"] = to_json(euclid_conf);
  js["parameter"] = anomaly_config;

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

TEST_F(anomaly_test, small) {
  jubatus::client::anomaly c("localhost", PORT, 10);

  {
    datum d;
    d.nv.push_back(make_pair("f1", 1.0));

    c.add(NAME, d);  // is it good to be inf?
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

}  // namespace
