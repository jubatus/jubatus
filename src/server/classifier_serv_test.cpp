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

#include <gtest/gtest.h>
#include <cmath>
#include <cfloat>
#include <map>

#include <pficommon/text/json.h>
#include <pficommon/lang/cast.h>

#include "classifier_serv.hpp"
#ifdef HAVE_ZOOKEEPER_H
#  include "mixer.hpp"
#endif
#include "../storage/storage_factory.hpp"
#include "../classifier/classifier_test_util.hpp"
#include "../storage/storage_type.hpp"

using namespace std;
using namespace jubatus;
using namespace jubatus::storage;
using namespace pfi::lang;
using namespace pfi::text::json;

static const string NAME = "test";

void make_small_data() {
  datum pos1;
  pos1.string_values_.push_back(make_pair("text", "a b c"));

  datum neg1;
  neg1.string_values_.push_back(make_pair("text", "A B C"));
}

datum convert_vector(const vector<double>& vec) {
  datum d;
  for (size_t i = 0; i < vec.size(); i++) {
    string f = "f" + lexical_cast<string>(i);
    d.num_values_.push_back(make_pair(f, vec[i]));
  }
  return d;
}

void make_random_data(vector<pair<string, datum> >& data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    pair<string, vector<double> > p = gen_random_data();
    data.push_back(make_pair(p.first, convert_vector(p.second)));
  }
}

bool operator == (const classifier::config_data& lhs, const classifier::config_data& rhs){
  return ( lhs.method == rhs.method );
}

void load_config(classifier::config_data& c){
  ifstream ifs("./test_input/config.json");
  ifs >> via_json_with_default(c);
}

TEST(classifier_config_data, config) {
  classifier::config_data c;
  load_config(c);
  SUCCEED();
}

TEST(classifier_config_data, config2) {
  classifier::config_data c;
  try{
    ifstream ifs("./test_input/config_svmlight.json");
    ifs >> via_json_with_default(c);
  }catch(const std::exception& e){
    cout << e.what() << endl;
    FAIL();
  }
}

jubatus::classifier::server* setup_serv(const char* stor = "local"){
  shared_ptr<storage::storage_base> s(storage::storage_factory::create_storage(stor));
  server_argv a;
  return new jubatus::classifier::server(s, a);
}

TEST(serv, api_config) {
  shared_ptr<jubatus::classifier::server> serv(setup_serv());
  classifier::config_data to_set;
  result<classifier::config_data> to_get;
  load_config(to_set);
  result<int> res_set = serv->set_config(NAME, to_set);
  ASSERT_TRUE(res_set.success) << res_set.error;
  ASSERT_EQ(0, res_set.retval);

  to_get = serv->get_config(NAME);
  ASSERT_TRUE(to_get.success);
  EXPECT_TRUE(to_set == to_get.retval);
}

TEST(serv, api_train){
  shared_ptr<jubatus::classifier::server> serv(setup_serv());
  const size_t example_size = 1000;
  classifier::config_data c;
  load_config(c);
  serv->set_config(NAME, c);

  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  result<int> res = serv->train(NAME, data);
  ASSERT_TRUE(res.success);
}

void my_test(const char* meth, const char* stor){ //serv2, api_classify){
  shared_ptr<jubatus::classifier::server> serv(setup_serv(stor));
  const size_t example_size = 1000;
  classifier::config_data c;
  c.method = meth;
  num_rule rule = { "*", "num" };
  c.converter.num_rules.push_back(rule);

  serv->set_config(NAME, c);

  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  result<int> res = serv->train(NAME, data);
  ASSERT_TRUE(res.success);

  vector<string> labels;
  vector<datum>  datas;
  {
    vector<pair<string, datum> >::const_iterator it;
    for(it = data.begin(); it != data.end(); ++it){
      labels.push_back(it->first);
      datas.push_back(it->second);
    }
  }
  result<vector<classifier::estimate_results> > result = serv->classify(NAME, datas);
  ASSERT_TRUE(res.success);
  ASSERT_EQ(example_size, result.retval.size());
  ASSERT_EQ(data.size(), result.retval.size());

  vector<string>::const_iterator it0; //answers
  vector<classifier::estimate_results>::const_iterator it;
  size_t count = 0;
  for(it = result.retval.begin(), it0 = labels.begin();
      it != result.retval.end() && it0 != labels.end();
      ++it, ++it0){
    for (size_t i = 0; i < it->size(); ++i){
      vector<classifier::estimate_result>::const_iterator ite;
      // for(ite = it->begin(); ite != it->end(); ++ite){
      //   cout << "[" << ite->label_ << "]" <<  endl;
      // }
    }
    ASSERT_EQ(2u, it->size()); //estimate_results should have two label OK/NG
    string most0;
    double prob0 = DBL_MIN;
    vector<classifier::estimate_result>::const_iterator ite;
    for(ite = it->begin(); ite != it->end(); ++ite){
      // get most likely label
      if(prob0 < ite->prob_ || ite == it->begin()){
        prob0 = ite->prob_;
        most0 = ite->label_;
      }
    }
    if(*it0 == most0){
      count++;
    }
    // EXPECT_TRUE(*it0 == most0);
    if( *it0 != most0 ){ //FIXME
      cout << *it0  << "!=" << most0 << endl;
      for(ite = it->begin(); ite != it->end(); ++ite){
        cout << ite->label_ << "\t" << ite->prob_ << endl;
      }
    }
  }
  EXPECT_GE(count, result.retval.size()-10); //num of wrong classification should be less than 1%

  jubatus::result<map<pair<string, int>, map<string, string> > > status = serv->get_status(NAME);
  ASSERT_TRUE(status.success);
  ASSERT_EQ(1u, status.retval.size());
  EXPECT_GE(status.retval.begin()->second.size(), 2u);
}


TEST(api_classify, pa){ //needs parameterized test if want more
  my_test("PA",    "local");
}
TEST(api_classify, pa1){
  my_test("PA1",   "local");
}
TEST(api_classify, pa2){
  my_test("PA2",   "local");
}
TEST(api_classify, cw){
  //  my_test("CW",    "local");
}
TEST(api_classify, arow){
  my_test("AROW",  "local");
}
TEST(api_classify, nherd){
  my_test("NHERD", "local");
}


TEST(mix_parameter, trivial) {
  diffv sum;
  diffv diff;

  diff.count = 3;
  diff.v.push_back(make_pair("feature1", feature_val3_t()));
  diff.v[0].second.push_back(make_pair("class1", val3_t(1.0f, 1.0f, 1.0f)));
  diff.v[0].second.push_back(make_pair("class2", val3_t(0.5f, 1.5f, 1.0f)));

  classifier::mix_parameter(sum, diff);

  EXPECT_EQ(1u, sum.v.size());
  EXPECT_FLOAT_EQ(3.0, sum.v[0].second[0].second.v1);
  EXPECT_FLOAT_EQ(1.0, sum.v[0].second[0].second.v2);

  EXPECT_FLOAT_EQ(1.5, sum.v[0].second[1].second.v1);
  EXPECT_FLOAT_EQ(1.0, sum.v[0].second[1].second.v2);

  diff.count = 4;
  classifier::mix_parameter(sum, diff);

  EXPECT_EQ(1u, sum.v.size());
  EXPECT_FLOAT_EQ(7.0, sum.v[0].second[0].second.v1);
  EXPECT_FLOAT_EQ(1.0, sum.v[0].second[0].second.v2);

  EXPECT_FLOAT_EQ(3.5, sum.v[0].second[1].second.v1);
  EXPECT_FLOAT_EQ(1.0, sum.v[0].second[1].second.v2);
}

TEST(save_load, trivial){
  const char* meth = "PA";
  shared_ptr<storage::storage_base> st(storage::storage_factory::create_storage("local"));
  server_argv a;
  classifier::server serv(st, a);

  std::vector<std::pair<std::string,int> > v;

  const size_t example_size = 1000;
  classifier::config_data c;
  c.method = meth;
  num_rule rule = { "*", "num" };
  c.converter.num_rules.push_back(rule);

  result<int> res_config = serv.set_config(NAME, c);
  ASSERT_TRUE(res_config.success);
  ASSERT_EQ(0, res_config.retval);
  vector<pair<string, datum> > data;
  make_random_data(data, example_size);
  result<int> res_train = serv.train(NAME, data);
  ASSERT_TRUE(res_train.success);
  ASSERT_GT(res_train.retval, 0);
  {
    st->set3("a", "x", val3_t(1, 11, 111));
    st->set3("a", "y", val3_t(2, 22, 222));
    st->set3("a", "z", val3_t(3, 33, 333));
    st->set3("b", "x", val3_t(12, 1212, 121212));
    st->set3("b", "z", val3_t(45, 4545, 454545));
  }
  result<int> res_save = serv.save(NAME, "hoge", "huga");
  ASSERT_TRUE(res_save.success);
  ASSERT_EQ(0, res_save.retval);

  shared_ptr<storage::storage_base> s2(storage::storage_factory::create_storage(string("local")));

  classifier::server serv2(s2, server_argv());
  result<int> res_load = serv2.load(NAME, "hoge", "huga");
  ASSERT_TRUE(res_load.success) << res_load.error;
  ASSERT_EQ(0, res_load.retval);
  res_config = serv2.set_config(NAME, c);
  ASSERT_TRUE(res_config.success);
  ASSERT_EQ(0, res_config.retval);
  // {
  //   feature_val3_t mm;
  //     s2->get3("a", mm);
      
  //     sort(mm.begin(), mm.end());
      
  //     feature_val3_t exp;
  //     exp.push_back(make_pair("x", val3_t(1, 11, 111)));
  //     exp.push_back(make_pair("y", val3_t(2, 22, 222)));
  //     exp.push_back(make_pair("z", val3_t(3, 33, 333)));
  //     EXPECT_EQ(exp, mm);
  // }
  
  vector<string> labels;
  vector<datum>  datas;
  {
    vector<pair<string, datum> >::const_iterator it;
    for(it = data.begin(); it != data.end(); ++it){
      labels.push_back(it->first);
      datas.push_back(it->second);
    }
  }
  result<vector<classifier::estimate_results> > result = serv2.classify(NAME, datas);
  ASSERT_TRUE(result.success);
  ASSERT_EQ(example_size, result.retval.size());
  ASSERT_EQ(data.size(), result.retval.size());
  
  vector<string>::const_iterator it0; //answers
  vector<classifier::estimate_results>::const_iterator it;
  size_t count = 0;
  for(it = result.retval.begin(), it0 = labels.begin();
      it != result.retval.end() && it0 != labels.end();
      ++it, ++it0){
    for (size_t i = 0; i < it->size(); ++i){
      vector<classifier::estimate_result>::const_iterator ite;
      // for(ite = it->begin(); ite != it->end(); ++ite){
      //   cout << "[" << ite->label_ << "]" <<  endl;
      // }
    }
    ASSERT_EQ(2u, it->size()); //estimate_results should have two label OK/NG
    string most0;
    double prob0 = DBL_MIN;
    vector<classifier::estimate_result>::const_iterator ite;
    for(ite = it->begin(); ite != it->end(); ++ite){
      // get most likely label
      if(prob0 < ite->prob_ || ite == it->begin()){
        prob0 = ite->prob_;
        most0 = ite->label_;
      }
    }
    if(*it0 == most0){
      count++;
    }
  }
  EXPECT_GE(count, result.retval.size()-10); //num of wrong classification should be less than 1%

}
