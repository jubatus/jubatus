// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "jubatus/core/regression/regression_test_util.hpp"
#include "jubatus/core/fv_converter/datum.hpp"
#include "jubatus/core/fv_converter/converter_config.hpp"
#include "regression_client.hpp"

#include "test_util.hpp"

using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::numeric_limits;
using std::cout;
using std::endl;
using pfi::data::optional;
using pfi::lang::lexical_cast;

using jubatus::datum;
using jubatus::client::regression;

static const char* NAME = "regression_test";
static const int PORT = 65434;

namespace {

class regression_test : public ::testing::Test {
 protected:
  pid_t child_;

  regression_test() {
    child_ = fork_process("regression",
                          PORT,
                          "./test_input/config.regression.json");
  }

  virtual ~regression_test() {
    kill_process(child_);
  }

  virtual void restart_process() {
    kill_process(this->child_);
    this->child_ = fork_process("regression",
                                PORT,
                                "./test_input/config.regression.json");
  }
};

datum convert_vector(const vector<double>& vec) {
  datum d;
  for (size_t i = 0; i < vec.size(); i++) {
    string f = "f" + lexical_cast<string>(i);
    d.num_values.push_back(make_pair(f, vec[i]));
  }
  return d;
}

void make_random_data(vector<pair<float, datum> >& data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    pair<float, vector<double> > p = gen_random_data(1.0, 1.0, 10);
    data.push_back(make_pair(p.first, convert_vector(p.second)));
  }
}

string make_simple_config(const string& method) {
  pfi::text::json::json js(new pfi::text::json::json_object());
  js["method"] = pfi::text::json::json(
      new pfi::text::json::json_string(method));
  jubatus::core::fv_converter::converter_config config;
  jubatus::core::fv_converter::num_rule rule = {
    "*", optional<string>(), "num"
  };
  config.num_rules.push_back(rule);
  std::stringstream conv;
  conv << config_to_string(config);
  pfi::text::json::json jsc;
  conv >> jsc;
  js["converter"] = jsc;

  pfi::text::json::json param(new pfi::text::json::json_object());
  param["sensitivity"] = pfi::text::json::json(
      new pfi::text::json::json_float(0.1f));
  param["regularization_weight"] = pfi::text::json::json(
      new pfi::text::json::json_float(numeric_limits<float>::max()));
  js["parameter"] = param;

  std::stringstream ret;
  ret << pfi::text::json::pretty(js);

  return ret.str();
}

void my_test(const char* meth, const char* stor) {
  regression r("localhost", PORT, 10);

  const size_t example_size = 1000;
  // string c = make_simple_config(meth);

  // r.set_config(NAME, c);

  vector<pair<float, datum> > data;
  make_random_data(data, example_size);
  unsigned int res = r.train(NAME, data);
  ASSERT_TRUE(res == data.size());

  vector<float> values;
  vector<datum> datas;
  {
    vector<pair<float, datum> >::const_iterator it;
    for (it = data.begin(); it != data.end(); ++it) {
      values.push_back(it->first);
      datas.push_back(it->second);
    }
  }
  vector<float> result = r.estimate(NAME, datas);

  ASSERT_EQ(example_size, result.size());
  ASSERT_EQ(data.size(), result.size());

  vector<float>::const_iterator it0;  // answers
  vector<float>::const_iterator it;
  size_t count = 0;
  for (it = result.begin(), it0 = values.begin();
      it != result.end() && it0 != values.end(); ++it, ++it0) {
    if (fabs(*it0 - *it) < 2.0) {
      count++;
    }
  }

  // num of wrong classification should be less than 1%
  EXPECT_GE(count, result.size() - 10);
}

TEST_F(regression_test, pa) {
  my_test("PA", "local");
}

TEST_F(regression_test, small) {
  regression c("localhost", PORT, 10);

  cout << "train" << endl;
  vector<pair<float, datum> > data;
  datum d;
  d.num_values.push_back(make_pair("f1", 1.0));
  data.push_back(make_pair(10, d));
  c.train(NAME, data);

  cout << "get_status" << endl;
  map<string, map<string, string> > status = c.get_status(NAME);
  EXPECT_EQ(status.size(), 1u);
  for (map<string, map<string, string> >::const_iterator it = status.begin();
      it != status.end(); ++it) {
    EXPECT_GE(it->second.size(), 8u);
  }

  c.save("", NAME);
  c.load("", NAME);

  cout << "estimate" << endl;
  vector<datum> test;
  test.push_back(d);
  vector<float> res = c.estimate(NAME, test);
  cout << res.size() << endl;
}

TEST_F(regression_test, api_get_client) {
  jubatus::client::regression cli("localhost", PORT, 10);
  string to_get = cli.get_config(NAME);

  msgpack::rpc::client& conn = cli.get_client();
  EXPECT_NO_THROW(conn.close());
}

}  // namespace
