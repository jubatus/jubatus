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


#include "gtest/gtest.h"

#include <string>
#include <iostream>
#include <limits>

#include <pficommon/lang/cast.h>
#include <pficommon/text/json.h>

#include "diffv.hpp"
#include "classifier_client.hpp"
#include "classifier_serv.hpp"
#include "../storage/storage_type.hpp"
#include "../classifier/classifier_test_util.hpp"
#include "../fv_converter/converter_config.hpp"

#include "test_util.hpp"

using namespace pfi::lang;
using namespace pfi::text::json;

using namespace jubatus;
using namespace jubatus::client;
using namespace jubatus::storage;
using namespace std;

static const string NAME = "classifier_test";
static const int PORT = 65432;

void make_small_data() {
  datum pos1;
  pos1.sv.push_back(make_pair("text", "a b c"));

  datum neg1;
  neg1.sv.push_back(make_pair("text", "A B C"));
}

datum convert_vector(const vector<double>& vec) {
  datum d;
  for (size_t i = 0; i < vec.size(); i++) {
    string f = "f" + lexical_cast<string>(i);
    d.nv.push_back(make_pair(f, vec[i]));
  }
  return d;
}

void make_random_data(vector<pair<string, datum> >& data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    pair<string, vector<double> > p = gen_random_data();
    data.push_back(make_pair(p.first, convert_vector(p.second)));
  }
}

bool operator == (const jubatus::config_data& lhs, const jubatus::config_data& rhs){
  return ( lhs.method == rhs.method );
}

void load_config(jubatus::config_data& c){
  fv_converter::converter_config cc;
  ifstream ifs("./test_input/config.json");
  ifs >> via_json_with_default(cc);
  c.method = "PA";
  framework::convert<fv_converter::converter_config, converter_config>(cc, c.config);
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

  class classifier_test : public ::testing::TestWithParam<const char*> {
  protected:
    pid_t child_;

    classifier_test(){
      child_ = fork_process("classifier", PORT);
    };
    virtual ~classifier_test(){
      kill_process(child_);
    };
    virtual void restart_process(){

      kill_process(this->child_);
      this->child_ = fork_process("classifier", PORT);
    };
  };

 //todo: insert __LINE__ as original line number
#define ASSERT_THROW2(statement__, type__, what__) \
  try{ statement__; FAIL();         \
  }catch(type__& __e__){ ASSERT_STREQ(what__, __e__.what()); }

TEST_P(classifier_test, set_config_exception){
  classifier c("localhost", PORT, 10);
  jubatus::config_data config;
  config.method = "pa";
  ASSERT_THROW2(c.set_config("", config), std::exception, "unsupported method (pa)");
  //  ASSERT_THROW(c.set_config("", config), std::exception);
  config.method = "";
  ASSERT_THROW2(c.set_config("", config), std::exception, "unsupported method ()");
  //  ASSERT_THROW(c.set_config("", config), std::exception);
  config.method = "saitama";
  ASSERT_THROW2(c.set_config("", config), std::exception, "unsupported method (saitama)");
  //  ASSERT_THROW(c.set_config("", config), std::exception);
}

TEST_P(classifier_test, simple){
  
  classifier c("localhost", PORT, 10);
  {
    jubatus::config_data config;
    config.method = GetParam();
    
    c.set_config("", config);
    c.get_config("");
  }
  {
    datum d;
    vector<pair<string,datum> > v;
    v.push_back(make_pair<string,datum>("hoge", d));
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

TEST_P(classifier_test, config) {
  jubatus::config_data c;
  load_config(c);
  SUCCEED();
}

TEST_P(classifier_test, api_config) {
  classifier cli("localhost", PORT, 10);
  config_data to_set;
  config_data to_get;
  load_config(to_set);

  int res_set = cli.set_config(NAME, to_set);
  //  ASSERT_(res_set.success) << res_set.error;
  ASSERT_EQ(0, res_set);

  EXPECT_NO_THROW(to_get = cli.get_config(NAME));
  EXPECT_TRUE(to_set == to_get);
}

TEST_P(classifier_test, api_train){
  classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;
  config_data c;
  load_config(c);
  cli.set_config(NAME, c);

  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  unsigned int res = cli.train(NAME, data);
  ASSERT_EQ(data.size(), res);
}

TEST_P(classifier_test, api_classify){
  classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;
  config_data c;
  load_config(c);

  vector<datum>  datas; //for classify

  vector<pair<string, datum> > data; //for train
  make_random_data(data, example_size);

  ASSERT_THROW2(cli.classify(NAME, datas), std::exception, "config_not_set");
  ASSERT_THROW2(cli.train(NAME, data), std::exception, "config_not_set");

  cli.set_config(NAME, c);

  unsigned int res = cli.train(NAME, data);
  ASSERT_EQ(data.size(), res);
}

void my_test(const char* method) {
  classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;
  config_data c;
  c.method = method;
  num_rule rule = { "*", "num" };
  c.config.num_rules.push_back(rule);

  cli.set_config(NAME, c);

  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  unsigned int res = cli.train(NAME, data);
  ASSERT_EQ(data.size(), res);

  vector<string> labels;
  vector<datum>  datas;
  {
    vector<pair<string, datum> >::const_iterator it;
    for(it = data.begin(); it != data.end(); ++it){
      labels.push_back(it->first);
      datas.push_back(it->second);
    }
  }
  vector<vector<estimate_result> > result = cli.classify(NAME, datas);
//  ASSERT_TRUE(res.);
  ASSERT_EQ(example_size, result.size());
  ASSERT_EQ(data.size(), result.size());

  vector<string>::const_iterator it0; //answers
  vector<vector<estimate_result> >::const_iterator it;
  size_t count = 0;
  for(it = result.begin(), it0 = labels.begin();
      it != result.end() && it0 != labels.end();
      ++it, ++it0){
    for (size_t i = 0; i < it->size(); ++i){
      vector<estimate_result>::const_iterator ite;
      // for(ite = it->begin(); ite != it->end(); ++ite){
      //   cout << "[" << ite->label << "]" <<  endl;
      // }
    }
    ASSERT_EQ(2u, it->size()); //estimate_results should have two label OK/NG
    string most0;
    double prob0 = DBL_MIN;
    vector<estimate_result>::const_iterator ite;
    for(ite = it->begin(); ite != it->end(); ++ite){
      // get most likely label
      if(prob0 < ite->prob || ite == it->begin()){
        prob0 = ite->prob;
        most0 = ite->label;
      }
    }
    if(*it0 == most0){
      count++;
    }
    // EXPECT_TRUE(*it0 == most0);
    if( *it0 != most0 ){ //FIXME
      cout << *it0  << "!=" << most0 << endl;
      for(ite = it->begin(); ite != it->end(); ++ite){
        cout << ite->label << "\t" << ite->prob << endl;
      }
    }
  }
  EXPECT_GE(count, result.size()-10); //num of wrong classification should be less than 1%
}

TEST_P(classifier_test, my_test) {
  my_test(GetParam());
}

TEST_P(classifier_test, duplicated_keys){
  classifier cli("localhost", PORT, 10);
  config_data c;
  c.method = GetParam();
  num_rule rule = { "*", "num" };
  c.config.num_rules.push_back(rule);

  cli.set_config(NAME, c);

  pfi::math::random::mtrand rand(0);
  datum d;
  for (size_t k = 0; k < 10; ++k) {
    uint32_t dim = rand.next_int(100);
    pair<string, double> feature = make_pair(lexical_cast<string>(dim), 1.0);
    // add 100 duplicated values
    for (size_t j = 0; j < 100; ++j)
      d.nv.push_back(feature);
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
      d.nv.push_back(make_pair(lexical_cast<string>(i), 1.0));
    }
    vector<datum> data;
    data.push_back(d);
    vector<vector<estimate_result> > result = cli.classify(NAME, data);
    ASSERT_EQ(1u, result.size());
    // if the classifier could not learn properly, it estimates scores of labels to NaN and returns no results.
    ASSERT_EQ(2u, result[0].size());
  }
}

INSTANTIATE_TEST_CASE_P(classifier_test_instance,
                        classifier_test,
                        testing::Values("PA", "PA1", "PA2", "CW", "AROW", "NHERD"));

  //FIXME: can't link classifier_serv
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


TEST_P(classifier_test, get_status){
  classifier cli("localhost", PORT, 10);

  map<string,map<string,string> > status = cli.get_status(NAME);
  EXPECT_EQ(status.size(), 1u);
  for(map<string,map<string,string> >::const_iterator it = status.begin();
      it != status.end(); ++it){
    EXPECT_GE(it->second.size(), 8u);
  }
}
TEST_P(classifier_test, save_load){
  classifier cli("localhost", PORT, 10);
  const char* meth = GetParam();
  std::vector<std::pair<std::string,int> > v;

  const size_t example_size = 1000;
  config_data c;
  c.method = meth;
  num_rule rule = { "*", "num" };
  c.config.num_rules.push_back(rule);

  int res_config = cli.set_config(NAME, c);
  //  ASSERT_TRUE(res_config.success);
  ASSERT_EQ(0, res_config);
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
  EXPECT_EQ(6, atoi(count_str.c_str()));
}

string classify_and_get_label(classifier& cli, const datum& d) {
  vector<datum> data;
  data.push_back(d);
  return get_max_label(cli.classify(NAME, data)[0]);
}

TEST_P(classifier_test, save_load_2){
  classifier cli("localhost", PORT, 10);
  std::vector<std::pair<std::string,int> > v;

  // Setup
  config_data c;
  c.method = GetParam();
  num_rule rule = { "*", "num" };
  c.config.num_rules.push_back(rule);

  int res_config = cli.set_config(NAME, c);
  ASSERT_EQ(0, res_config);

  // Test data
  datum pos;
  pos.nv.push_back(make_pair("value", 10.0));
  datum neg;
  neg.nv.push_back(make_pair("value", -10.0));

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
  ASSERT_TRUE(pos_max == neg_max);

  // Reload server
  ASSERT_TRUE(cli.load(NAME, "test"));

  // The classifier works well
  ASSERT_EQ("pos", classify_and_get_label(cli, pos));
  ASSERT_EQ("neg", classify_and_get_label(cli, neg));
}

TEST_P(classifier_test, nan){
  classifier cli("localhost", PORT, 10);

  // Setup
  config_data c;
  c.method = GetParam();
  num_rule rule = { "*", "num" };
  c.config.num_rules.push_back(rule);

  int res_config = cli.set_config(NAME, c);
  ASSERT_EQ(0, res_config);

  datum d;
  d.nv.push_back(make_pair("value", numeric_limits<float>::quiet_NaN()));
  vector<pair<string, datum> > data;
  data.push_back(make_pair("l1", d));
  cli.train(NAME, data);

  vector<datum> test;
  test.push_back(d);
  vector<vector<estimate_result> > result = cli.classify(NAME, test);
  ASSERT_EQ(1u, result.size());
  ASSERT_EQ(1u, result[0].size());
  EXPECT_FALSE(isfinite(result[0][0].prob));
}

}

