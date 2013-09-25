// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "../server/diffv.hpp"
#include "window_classifier_client.hpp"
#include "window_classifier_serv.hpp"
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

static const string NAME = "window_classifier_test";
static const int PORT = 65437;
static const string WIN_ID = "win_id";

datum convert_vector(const vector<double>& vec) {
  datum d;
  for (size_t i = 0; i < vec.size(); i++) {
    string f = "f" + lexical_cast<string>(i);
    d.num_values.push_back(make_pair(f, vec[i]));
  }
  return d;
}

void make_random_data(vector<pair<string, datum> >& data, size_t size) {
  // const float mu_pos = 10.0;
  // const float mu_neg = -10.0;
  const float sigma = 0.001;
  const size_t dim = 1;

  float mu = 0;
  double diff = 1;
  string label;
  for (size_t i = 0; i < size; ++i) {
    if (mu > 10) {
      diff = -1;
    } else if (mu < -10) {
      diff = 1;
    }
    
    if (diff > 0) {
      label = "OK";
    } else {
      label = "NG";
    }
    
    mu += diff;
    vector<double> v;
    make_random(mu, sigma, dim, v);
    data.push_back(make_pair(label, convert_vector(v)));
  }
}

void make_random_data(vector<datum>& data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    pair<string, vector<double> > p = gen_random_data();
    data.push_back(convert_vector(p.second));
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

std::string make_simple_config(const string& method) {
  pfi::text::json::json js(new pfi::text::json::json_object());
  js["method"] = pfi::text::json::json(
      new pfi::text::json::json_string(method));
  jubatus::fv_converter::converter_config config;
  pfi::data::optional<std::string> noexception("");
  jubatus::fv_converter::num_rule rule = { "*", noexception,"num" };
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

void load_config(string& c){
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

  // c.window_config = "[{\"key\" : \"*\", \"size\" : 3, \"suffix\" : \"+win\"}]";
}

string get_max_label(const vector<estimate_result>& result) {
  string max_label = "";
  double max_prob = 0;
  for (size_t i = 0; i < result.size(); ++i) {
    if (max_label == "" || result[i].prob > max_prob) {
      max_label = result[i].label;
      max_prob = result[i].prob;
    }
  }
  return max_label;
}

namespace {

class window_classifier_test : public ::testing::TestWithParam<const char*> {
protected:
  pid_t child_;

  window_classifier_test() {
    std::string config_path =
      "./test_input/config.classifier." + std::string(GetParam()) + ".json";
    child_ = fork_process("window_classifier", PORT, config_path);
  };
  virtual ~window_classifier_test() {
    kill_process(child_);
  };
  virtual void restart_process() {
    kill_process(this->child_);
    std::string config_path =
      "./test_input/config.classifier." + std::string(GetParam()) + ".json";
    this->child_ = fork_process("window_classifier", PORT, config_path);
  };
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

TEST_P(window_classifier_test, simple) {
  jubatus::client::window_classifier c("localhost", PORT, 10);
  {
    datum d;
    vector<pair<string,datum> > v;
    v.push_back(make_pair("hoge", d));
    c.train("", WIN_ID, v);

    c.classify("", WIN_ID, d);
  }
  {
    c.save("", "test");
    c.load("", "test");
  }
}

TEST_P(window_classifier_test, api_config) {
  jubatus::client::window_classifier cli("localhost", PORT, 10);
  string to_get;

  EXPECT_NO_THROW(to_get = cli.get_config(NAME));
}

TEST_P(window_classifier_test, api_train) {
  jubatus::client::window_classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;
  
  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  unsigned int res = cli.train(NAME, WIN_ID, data);
  ASSERT_EQ(data.size(), res);
}

TEST_P(window_classifier_test, api_classify) {
  jubatus::client::window_classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;

  vector<datum>  datas; //for classify

  vector<pair<string, datum> > data; //for train
  make_random_data(data, example_size);

  unsigned int res = cli.train(NAME, WIN_ID, data);
  ASSERT_EQ(data.size(), res);
}

TEST_P(window_classifier_test, api_push) {
  jubatus::client::window_classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;

  vector<datum> data;
  make_random_data(data, example_size);
  unsigned int res = cli.push(NAME, WIN_ID, data);
  ASSERT_EQ(data.size(), res);
}

TEST_P(window_classifier_test, api_clear_window) {
  jubatus::client::window_classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;

  cli.clear_window(NAME, WIN_ID);

  vector<datum> data;
  make_random_data(data, example_size);
  cli.push(NAME, WIN_ID, data);

  cli.clear_window(NAME, WIN_ID);
}

void my_test(const char* method) {
  jubatus::client::window_classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;
  vector<pair<string, datum> > data;
  make_random_data(data, example_size);

  vector<datum> fill_data;
  {
    vector<pair<string, datum> >::iterator it = data.begin();
    fill_data.push_back(it->second);
    ++it;
    fill_data.push_back(it->second);
    cli.push(NAME, WIN_ID, fill_data);
  }
  vector<pair<string, datum> > train_data(data.begin()+2, data.end());
  unsigned int res = cli.train(NAME, WIN_ID, train_data);
  EXPECT_EQ(data.size()-2, res);


  // vector<string> labels;
  // vector<datum>  datas;
  // {
  //   vector<pair<string, datum> >::const_iterator it;
  //   for(it = data.begin(); it != data.end(); ++it){
  //     labels.push_back(it->first);
  //     datas.push_back(it->second);
  //   }
  // }
  // vector<vector<estimate_result> > result = cli.classify(NAME, WIN_ID, datas, 0);
  // ASSERT_EQ(example_size, result.size());
  // ASSERT_EQ(data.size(), result.size());

  // vector<string>::const_iterator it0; //answers
  // vector<vector<estimate_result> >::const_iterator it;
  size_t count = 0;
  // for(it = result.begin(), it0 = labels.begin();
  //     it != result.end() && it0 != labels.end(); ++it, ++it0){
  cli.clear_window(NAME, WIN_ID);
  cli.push(NAME, WIN_ID, fill_data);
  for(vector<pair<string, datum> >::const_iterator it = train_data.begin(); it != train_data.end(); ++it) {

    vector<datum> send_data;
    send_data.push_back(it->second);
    vector<estimate_result> result = cli.classify(NAME, WIN_ID, it->second);
    cli.push(NAME, WIN_ID, send_data);

    ASSERT_EQ(2u, result.size()); //estimate_results should have two label OK/NG
    string most0;
    double prob0 = DBL_MIN;
    vector<estimate_result>::const_iterator ite;
    for (ite = result.begin(); ite != result.end(); ++ite) {
      // get most likely label
      if (prob0 < ite->prob || ite == result.begin()) {
        prob0 = ite->prob;
        most0 = ite->label;
      }
    }
    if (most0.compare(it->first) == 0) {
      count++;
    }
    // EXPECT_TRUE(*it0 == most0);
    if (most0.compare(it->first) != 0) {
      cout << it->first << "!=" << most0 << endl;
      for (ite = result.begin(); ite != result.end(); ++ite) {
        cout << ite->label << "\t" << ite->prob << endl;
      }
    }
  }
  //num of wrong classification should be less than 1%
  EXPECT_GE(count, data.size()-10);
}

TEST_P(window_classifier_test, my_test) {
  my_test(GetParam());
}

TEST_P(window_classifier_test, duplicated_keys){
  jubatus::client::window_classifier cli("localhost", PORT, 10);

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
    cli.train(NAME, WIN_ID, data);
  }

  {
    datum d;
    for (size_t i = 0; i < 100; ++i) {
      d.num_values.push_back(make_pair(lexical_cast<string>(i), 1.0));
    }
    vector<estimate_result> result = cli.classify(NAME, WIN_ID, d);
    // if the classifier could not learn properly, it estimates scores of labels to NaN and returns no results.
    ASSERT_EQ(2u, result.size());
  }
}

INSTANTIATE_TEST_CASE_P(window_classifier_test_instance,
                        window_classifier_test,
                        testing::Values("PA", "PA1", "PA2", "CW", "AROW", "NHERD"));

TEST_P(window_classifier_test, get_status){
  jubatus::client::window_classifier cli("localhost", PORT, 10);

  map<string,map<string,string> > status = cli.get_status(NAME);
  EXPECT_EQ(status.size(), 1u);
  for(map<string,map<string,string> >::const_iterator it = status.begin();
      it != status.end(); ++it){
    EXPECT_GE(it->second.size(), 8u);
  }
}
TEST_P(window_classifier_test, save_load){
  jubatus::client::window_classifier cli("localhost", PORT, 10);
  std::vector<std::pair<std::string,int> > v;

  const size_t example_size = 1000;

  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  unsigned int res_train = cli.train(NAME, WIN_ID, data);

  EXPECT_EQ(data.size(), res_train);
  bool res_save = cli.save(NAME, "hoge");
  EXPECT_EQ(true, res_save);

  bool res_load = cli.load(NAME, "hoge");
  ASSERT_EQ(true, res_load);
  cli.clear_window(NAME, WIN_ID);
  my_test(GetParam());

  map<string, map<string, string> > status = cli.get_status(NAME);
  string count_str = status.begin()->second["update_count"];
  // EXPECT_EQ(7, atoi(count_str.c_str())); // 
  EXPECT_EQ(1006, atoi(count_str.c_str())); // TODO: right?
}

string classify_and_get_label(
    jubatus::client::window_classifier& cli,
    const datum& d) {
  return get_max_label(cli.classify(NAME, WIN_ID, d));
}

TEST_P(window_classifier_test, save_load_2){
  jubatus::client::window_classifier cli("localhost", PORT, 10);
  std::vector<std::pair<std::string,int> > v;

  // Test data
  datum pos;
  pos.num_values.push_back(make_pair("value", 10.0));
  datum neg;
  neg.num_values.push_back(make_pair("value", -10.0));

  // Save empty state
  ASSERT_TRUE(cli.save(NAME, "empty"));

  // Train
  vector<datum> data;
  data.push_back(neg);
  data.push_back(neg);
  cli.push(NAME, WIN_ID, data);

  vector<pair<string, datum> > train_data;
  train_data.push_back(make_pair("pos", pos));
  unsigned int res_train = cli.train(NAME, WIN_ID, train_data);
  ASSERT_EQ(1u, res_train);
  
  cli.push(NAME, WIN_ID, data);
  train_data.clear();
  train_data.push_back(make_pair("neg", neg));
  res_train = cli.train(NAME, WIN_ID, train_data);
  ASSERT_EQ(1u, res_train);

  // Now, the classifier can classify properly
  vector<datum> pos_v;
  pos_v.push_back(pos);
  cli.push(NAME, WIN_ID, pos_v);
  cli.push(NAME, WIN_ID, pos_v);
  ASSERT_EQ("pos", classify_and_get_label(cli, pos));
  cli.push(NAME, WIN_ID, pos_v);
  vector<datum> neg_v;
  neg_v.push_back(neg);
  cli.push(NAME, WIN_ID, neg_v);
  cli.push(NAME, WIN_ID, neg_v);
  ASSERT_EQ("neg", classify_and_get_label(cli, neg));
  cli.push(NAME, WIN_ID, neg_v);

  // Save current state
  ASSERT_TRUE(cli.save(NAME, "test"));

  // Load empty
  ASSERT_TRUE(cli.load(NAME, "empty"));

  // And the classifier classify data improperly, but cannot expect results
  string pos_max = classify_and_get_label(cli, pos);
  string neg_max = classify_and_get_label(cli, neg);
  ASSERT_TRUE(pos_max == neg_max);

  // Reload server
  ASSERT_TRUE(cli.load(NAME, "test"));

  // The classifier works well
  ASSERT_EQ("pos", classify_and_get_label(cli, pos));
  ASSERT_EQ("neg", classify_and_get_label(cli, neg));
}

TEST_P(window_classifier_test, nan){
  jubatus::client::window_classifier cli("localhost", PORT, 10);

  datum d;
  d.num_values.push_back(make_pair("value", numeric_limits<float>::quiet_NaN()));
  vector<pair<string, datum> > data;
  data.push_back(make_pair("l1", d));
  cli.train(NAME, WIN_ID, data);

  cli.clear_window(NAME, WIN_ID);
  vector<estimate_result> result = cli.classify(NAME, WIN_ID, d);
  ASSERT_EQ(1u, result.size());
  EXPECT_FALSE(isfinite(result[0].prob));
}

}

