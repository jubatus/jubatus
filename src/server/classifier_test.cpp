// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include <pficommon/lang/cast.h>
#include <pficommon/text/json.h>

#include "classifier_client.hpp"
#include "classifier_serv.hpp"
#include "../storage/storage_type.hpp"
#include "../classifier/classifier_test_util.hpp"
#include "../fv_converter/converter_config.hpp"

#include "test_util.hpp"

using std::string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::ifstream;
using std::stringstream;
using std::isfinite;
using std::numeric_limits;

using pfi::lang::lexical_cast;
using jubatus::datum;
using jubatus::estimate_result;

static const char* NAME = "classifier_test";
static const int PORT = 65432;

void make_small_data() {
  datum pos1;
  pos1.string_values.push_back(make_pair("text", "a b c"));

  datum neg1;
  neg1.string_values.push_back(make_pair("text", "A B C"));
}

datum convert_vector(const vector<double>& vec) {
  datum d;
  for (size_t i = 0; i < vec.size(); i++) {
    string f = "f" + lexical_cast<string>(i);
    d.num_values.push_back(make_pair(f, vec[i]));
  }
  return d;
}

void make_random_data(vector<pair<string, datum> >& data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    pair<string, vector<double> > p = gen_random_data();
    data.push_back(make_pair(p.first, convert_vector(p.second)));
  }
}

void insert_parameter(const string& method, pfi::text::json::json& js) {
  pfi::text::json::json param(new pfi::text::json::json_object());
  // classifier's parameter
  if (!(method == "perceptron" || method == "PA")) {
    float c = 1.001f;
    if (method == "NHERD") {
      c = 0.1f;
    }
    param["regularization_weight"] = pfi::text::json::json(
        new pfi::text::json::json_float(c));
    js["parameter"] = param;
  }
}

string make_simple_config(const string& method) {
  pfi::text::json::json js(new pfi::text::json::json_object());
  js["method"] = pfi::text::json::json(
      new pfi::text::json::json_string(method));
  jubatus::fv_converter::converter_config config;
  jubatus::fv_converter::num_rule rule = { "*", "num" };
  config.num_rules.push_back(rule);
  std::stringstream conv;
  conv << config_to_string(config);
  pfi::text::json::json jsc;
  conv >> jsc;
  js["converter"] = jsc;
  insert_parameter(method, js);

  std::stringstream ret;
  ret << pfi::text::json::pretty(js);

  return ret.str();
}

string make_empty_config(const string& method) {
  pfi::text::json::json js(new pfi::text::json::json_object());
  js["method"] = pfi::text::json::json(
      new pfi::text::json::json_string(method));
  jubatus::fv_converter::converter_config config;
  std::stringstream conv;
  conv << config_to_string(config);
  pfi::text::json::json jsc;
  conv >> jsc;
  js["converter"] = jsc;
  insert_parameter(method, js);

  std::stringstream ret;
  ret << pfi::text::json::pretty(js);

  return ret.str();
}

void load_config(string& c) {
  ifstream ifs("./test_input/config.json");
  stringstream ss;
  ss << ifs.rdbuf();

  // insert parameter
  pfi::text::json::json js;
  ss >> js;
  pfi::text::json::json param(new pfi::text::json::json_object());
  const string method = pfi::text::json::json_cast<string>(js["method"]);
  insert_parameter(method, js);

  std::stringstream ret;
  ret << pfi::text::json::pretty(js);
  c = ret.str();
}

string get_max_label(const vector<estimate_result>& result) {
  string max_label = "";
  double max_prob = 0;
  for (size_t i = 0; i < result.size(); ++i) {
    if (max_label == "" || result[i].score > max_prob) {
      max_label = result[i].label;
      max_prob = result[i].score;
    }
  }
  return max_label;
}

namespace {

class classifier_test : public ::testing::TestWithParam<const char*> {
 protected:
  pid_t child_;

  classifier_test() {
    std::string config_path =
        "./test_input/config.classifier." + std::string(GetParam()) + ".json";
    child_ = fork_process("classifier", PORT, config_path);
  }

  virtual ~classifier_test() {
    kill_process(child_);
  }

  virtual void restart_process() {
    kill_process(this->child_);
    std::string config_path =
        "./test_input/config.classifier." + std::string(GetParam()) + ".json";
    this->child_ = fork_process("classifier", PORT, config_path);
  }
};

// todo: insert __LINE__ as original line number
#define ASSERT_THROW2(statement__, type__, what__) \
  try { \
    statement__; \
    FAIL(); \
  } catch (type__& __e__) { \
    ASSERT_STREQ(what__, __e__.what()); \
  }

/*
 TEST_P(classifier_test, set_config_exception){
 jubatus::client::classifier c("localhost", PORT, 10);
 string config = make_empty_config("pa");
 ASSERT_THROW2(c.set_config("", config), std::exception, "unsupported method (pa)");
 //  ASSERT_THROW(c.set_config("", config), std::exception);
 config = make_empty_config("");
 ASSERT_THROW2(c.set_config("", config), std::exception, "unsupported method ()");
 //  ASSERT_THROW(c.set_config("", config), std::exception);
 config = make_empty_config("saitama");
 ASSERT_THROW2(c.set_config("", config), std::exception, "unsupported method (saitama)");
 //  ASSERT_THROW(c.set_config("", config), std::exception);
 }
 */

TEST_P(classifier_test, simple) {
  jubatus::client::classifier c("localhost", PORT, 10);
  {
    datum d;
    vector<pair<string, datum> > v;
    v.push_back(make_pair("hoge", d));
    c.train("", v);

    vector<datum> v2;
    v2.push_back(d);
    c.classify("", v2);
  }
  {
    c.save("", "test");
    c.load("", "test");
  }
}

TEST_P(classifier_test, api_config) {
  jubatus::client::classifier cli("localhost", PORT, 10);
  string to_get;

  EXPECT_NO_THROW(to_get = cli.get_config(NAME));
}

TEST_P(classifier_test, api_train) {
  jubatus::client::classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;
  string c;

  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  unsigned int res = cli.train(NAME, data);
  ASSERT_EQ(data.size(), res);
}

TEST_P(classifier_test, api_classify) {
  jubatus::client::classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;

  vector<datum> datas;  // for classify

  vector<pair<string, datum> > data;  // for train
  make_random_data(data, example_size);

  unsigned int res = cli.train(NAME, data);
  ASSERT_EQ(data.size(), res);
}

void my_test(const char* method) {
  jubatus::client::classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;

  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  unsigned int res = cli.train(NAME, data);
  ASSERT_EQ(data.size(), res);

  vector<string> labels;
  vector<datum> datas;
  {
    vector<pair<string, datum> >::const_iterator it;
    for (it = data.begin(); it != data.end(); ++it) {
      labels.push_back(it->first);
      datas.push_back(it->second);
    }
  }
  vector<vector<estimate_result> > result = cli.classify(NAME, datas);
//  ASSERT_TRUE(res.);
  ASSERT_EQ(example_size, result.size());
  ASSERT_EQ(data.size(), result.size());

  vector<string>::const_iterator it0;  // answers
  vector<vector<estimate_result> >::const_iterator it;
  size_t count = 0;
  for (it = result.begin(), it0 = labels.begin();
      it != result.end() && it0 != labels.end(); ++it, ++it0) {
    for (size_t i = 0; i < it->size(); ++i) {
      vector<estimate_result>::const_iterator ite;
      // for(ite = it->begin(); ite != it->end(); ++ite){
      //   cout << "[" << ite->label << "]" <<  endl;
      // }
    }
    ASSERT_EQ(2u, it->size());  // estimate_results should have two label OK/NG
    string most0;
    double prob0 = DBL_MIN;
    vector<estimate_result>::const_iterator ite;
    for (ite = it->begin(); ite != it->end(); ++ite) {
      // get most likely label
      if (prob0 < ite->score || ite == it->begin()) {
        prob0 = ite->score;
        most0 = ite->label;
      }
    }
    if (most0.compare(*it0) == 0) {
      count++;
    }
    // EXPECT_TRUE(*it0 == most0);
    if (most0.compare(*it0) != 0) {
      cout << *it0 << "!=" << most0 << endl;
      for (ite = it->begin(); ite != it->end(); ++ite) {
        cout << ite->label << "\t" << ite->score << endl;
      }
    }
  }
  // num of wrong classification should be less than 1%
  EXPECT_GE(count, result.size() - 10);
}

TEST_P(classifier_test, my_test) {
  my_test(GetParam());
}

TEST_P(classifier_test, duplicated_keys) {
  jubatus::client::classifier cli("localhost", PORT, 10);

  pfi::math::random::mtrand rand(0);
  datum d;
  for (size_t k = 0; k < 10; ++k) {
    uint32_t dim = rand.next_int(100);
    pair<string, double> feature = make_pair(lexical_cast<string>(dim), 1.0);
    // add 100 duplicated values
    for (size_t j = 0; j < 100; ++j)
    d.num_values.push_back(feature);
  }
  vector<pair<string, datum> > data;
  data.push_back(make_pair("", d));
  for (size_t i = 0; i < 100; ++i) {
    data[0].first = i % 2 == 0 ? "P" : "N";
    cli.train(NAME, data);
  }

  {
    datum d;
    for (size_t i = 0; i < 100; ++i) {
      d.num_values.push_back(make_pair(lexical_cast<string>(i), 1.0));
    }
    vector<datum> data;
    data.push_back(d);
    vector<vector<estimate_result> > result = cli.classify(NAME, data);
    ASSERT_EQ(1u, result.size());
    /* if the classifier could not learn properly, it estimates
       scores of labels to NaN and returns no results. */
    ASSERT_EQ(2u, result[0].size());
  }
}

INSTANTIATE_TEST_CASE_P(classifier_test_instance,
    classifier_test,
    testing::Values("PA", "PA1", "PA2", "CW", "AROW", "NHERD"));

// TODO(kuenishi): can't link classifier_serv
// TEST(mix_parameter, trivial) {
//   diffv sum;
//   diffv diff;

//   diff.count = 3;
//   diff.v.push_back(make_pair("feature1", feature_val3_t()));
//   diff.v[0].second.push_back(make_pair("class1", val3_t(1.0f, 1.0f, 1.0f)));
//   diff.v[0].second.push_back(make_pair("class2", val3_t(0.5f, 1.5f, 1.0f)));

//   server::mix_parameter(sum, diff);

//   EXPECT_EQ(1u, sum.v.size());
//   EXPECT_FLOAT_EQ(3.0, sum.v[0].second[0].second.v1);
//   EXPECT_FLOAT_EQ(1.0, sum.v[0].second[0].second.v2);

//   EXPECT_FLOAT_EQ(1.5, sum.v[0].second[1].second.v1);
//   EXPECT_FLOAT_EQ(1.0, sum.v[0].second[1].second.v2);

//   diff.count = 4;
//   server::mix_parameter(sum, diff);

//   EXPECT_EQ(1u, sum.v.size());
//   EXPECT_FLOAT_EQ(7.0, sum.v[0].second[0].second.v1);
//   EXPECT_FLOAT_EQ(1.0, sum.v[0].second[0].second.v2);

//   EXPECT_FLOAT_EQ(3.5, sum.v[0].second[1].second.v1);
//   EXPECT_FLOAT_EQ(1.0, sum.v[0].second[1].second.v2);
// }

TEST_P(classifier_test, get_status) {
  jubatus::client::classifier cli("localhost", PORT, 10);

  map<string, map<string, string> > status = cli.get_status(NAME);
  EXPECT_EQ(status.size(), 1u);
  for (map<string, map<string, string> >::const_iterator it = status.begin();
      it != status.end(); ++it) {
    EXPECT_GE(it->second.size(), 8u);
  }
}

TEST_P(classifier_test, save_load) {
  jubatus::client::classifier cli("localhost", PORT, 10);
  std::vector<std::pair<std::string, int> > v;

  const size_t example_size = 1000;
//  string c = make_simple_config(GetParam());

  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  unsigned int res_train = cli.train(NAME, data);

  ASSERT_EQ(data.size(), res_train);
  // {
  //   st->set3("a", "x", val3_t(1, 11, 111));
  //   st->set3("a", "y", val3_t(2, 22, 222));
  //   st->set3("a", "z", val3_t(3, 33, 333));
  //   st->set3("b", "x", val3_t(12, 1212, 121212));
  //   st->set3("b", "z", val3_t(45, 4545, 454545));
  // }
  bool res_save = cli.save(NAME, "hoge");
  ASSERT_EQ(true, res_save);

  bool res_load = cli.load(NAME, "hoge");
  ASSERT_EQ(true, res_load);
  my_test(GetParam());

  map<string, map<string, string> > status = cli.get_status(NAME);
  string count_str = status.begin()->second["update_count"];
  EXPECT_EQ(4, atoi(count_str.c_str()));
}

string classify_and_get_label(
    jubatus::client::classifier& cli,
    const datum& d) {
  vector<datum> data;
  data.push_back(d);
  return get_max_label(cli.classify(NAME, data)[0]);
}

TEST_P(classifier_test, save_load_2) {
  jubatus::client::classifier cli("localhost", PORT, 10);
  std::vector<std::pair<std::string, int> > v;

  // Test data
  datum pos;
  pos.num_values.push_back(make_pair("value", 10.0));
  datum neg;
  neg.num_values.push_back(make_pair("value", -10.0));

  // Save empty state
  ASSERT_TRUE(cli.save(NAME, "empty"));

  // Train
  vector<pair<string, datum> > data;
  data.push_back(make_pair("pos", pos));
  data.push_back(make_pair("neg", neg));
  unsigned int res_train = cli.train(NAME, data);
  ASSERT_EQ(data.size(), res_train);

  // Now, the classifier can classify properly
  ASSERT_EQ("pos", classify_and_get_label(cli, pos));
  ASSERT_EQ("neg", classify_and_get_label(cli, neg));

  // Save current state
  ASSERT_TRUE(cli.save(NAME, "test"));

  // Load empty
  ASSERT_TRUE(cli.load(NAME, "empty"));

  // And the classifier classify data improperly, but cannot expect results
  string pos_max = classify_and_get_label(cli, pos);
  string neg_max = classify_and_get_label(cli, neg);
  ASSERT_EQ(0, pos_max.compare(neg_max));

  // Reload server
  ASSERT_TRUE(cli.load(NAME, "test"));

  // The classifier works well
  ASSERT_EQ("pos", classify_and_get_label(cli, pos));
  ASSERT_EQ("neg", classify_and_get_label(cli, neg));
}

TEST_P(classifier_test, nan) {
  jubatus::client::classifier cli("localhost", PORT, 10);

  datum d;
  d.num_values.push_back(
      make_pair("value", numeric_limits<float>::quiet_NaN()));
  vector<pair<string, datum> > data;
  data.push_back(make_pair("l1", d));
  cli.train(NAME, data);

  vector<datum> test;
  test.push_back(d);
  vector<vector<estimate_result> > result = cli.classify(NAME, test);
  ASSERT_EQ(1u, result.size());
  ASSERT_EQ(1u, result[0].size());
  EXPECT_FALSE(isfinite(result[0][0].score));
}

}  // namespace
