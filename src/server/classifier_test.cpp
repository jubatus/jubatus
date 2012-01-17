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

#include <string>
#include <iostream>

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

static const string NAME = "test";
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
  convert<fv_converter::converter_config, converter_config>(cc, c.config);
}

namespace {

  class classifier_test : public ::testing::Test {
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

TEST_F(classifier_test, simple){
  
  classifier c("localhost", PORT, 10);
  {
    jubatus::config_data config;
    config.method = "PA";
    
    c.set_config("", config);
    c.get_config("", 0);
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

TEST_F(classifier_test, config) {
  jubatus::config_data c;
  load_config(c);
  SUCCEED();
}

TEST_F(classifier_test, api_config) {
  classifier cli("localhost", PORT, 10);
  config_data to_set;
  config_data to_get;
  load_config(to_set);
  int res_set = cli.set_config(NAME, to_set);
  //  ASSERT_(res_set.success) << res_set.error;
  ASSERT_EQ(0, res_set);

  to_get = cli.get_config(NAME, 0);
  //  ASSERT_TRUE(to_get.success);
  EXPECT_TRUE(to_set == to_get);
}

TEST_F(classifier_test, api_train){
  classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;
  config_data c;
  load_config(c);
  cli.set_config(NAME, c);

  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  unsigned int res = cli.train(NAME, data);
  ASSERT_TRUE(res == data.size()); //.success);
}

void my_test(const char* meth, const char* stor){ //serv2, api_classify){
  classifier cli("localhost", PORT, 10);
  const size_t example_size = 1000;
  config_data c;
  c.method = meth;
  num_rule rule = { "*", "num" };
  c.config.num_rules.push_back(rule);

  cli.set_config(NAME, c);

  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  unsigned int res = cli.train(NAME, data);
  ASSERT_TRUE(res == data.size()); //.success);

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


TEST_F(classifier_test, pa){ //needs parameterized test if want more
  this->restart_process();
  my_test("PA",    "local");
}
TEST_F(classifier_test, pa1){
  my_test("PA1",   "local");
}
TEST_F(classifier_test, pa2){
  my_test("PA2",   "local");
}
TEST_F(classifier_test, cw){
  //  my_test("CW",    "local");
}
TEST_F(classifier_test, arow){
  my_test("AROW",  "local");
}
TEST_F(classifier_test, nherd){
  my_test("NHERD", "local");
}

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

TEST_F(classifier_test, get_status){
  classifier cli("localhost", PORT, 10);
  map<string,map<string,string> > status = cli.get_status(NAME, 0);
  EXPECT_EQ(status.size(), 1u);
  for(map<string,map<string,string> >::const_iterator it = status.begin();
      it != status.end(); ++it){
    EXPECT_GE(it->second.size(), 8u);
  }
}
TEST_F(classifier_test, save_load){
  classifier cli("localhost", PORT, 10);
  const char* meth = "PA";
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
  int res_train = cli.train(NAME, data);
  //  ASSERT_TRUE(res_train.success);
  ASSERT_GT(res_train, 0);
  // {
  //   st->set3("a", "x", val3_t(1, 11, 111));
  //   st->set3("a", "y", val3_t(2, 22, 222));
  //   st->set3("a", "z", val3_t(3, 33, 333));
  //   st->set3("b", "x", val3_t(12, 1212, 121212));
  //   st->set3("b", "z", val3_t(45, 4545, 454545));
  // }
  int res_save = cli.save(NAME, "hoge");
  //  ASSERT_TRUE(res_save.success);
  ASSERT_EQ(0, res_save);

  int res_load = cli.load(NAME, "hoge");
  // classifier::server serv2(s2, server_argv());
  // result<int> res_load = serv2.load(NAME, "hoge", "huga");
  // ASSERT_TRUE(res_load.success) << res_load.error;
  ASSERT_EQ(0, res_load);

  my_test("PA",    "local");
  map<string, map<string, string> > status = cli.get_status(NAME, 0);
  string count_str = status.begin()->second["update_count"];
  EXPECT_EQ(5, atoi(count_str.c_str()));
}

}

