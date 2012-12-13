#include "gtest/gtest.h"
#include "recommender_client.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/converter_config.hpp"

#include "../recommender/recommender.hpp"
#include "../recommender/recommender_type.hpp"
#include "../classifier/classifier_test_util.hpp"
#include "../common/exception.hpp"

#include <pficommon/lang/cast.h>
#include <vector>
#include "test_util.hpp"

using namespace std;
using namespace pfi::lang;
using namespace jubatus;
using jubatus::client::recommender;

static const string NAME = "test";
static const int PORT = 65433;

namespace {

  class recommender_test : public ::testing::Test {
  protected:
    pid_t child_;

    recommender_test(){
      child_ = fork_process("recommender", PORT);
    };
    virtual ~recommender_test(){
      kill_process(child_);
    };
    virtual void restart_process(){

      kill_process(this->child_);
      this->child_ = fork_process("recommender", PORT);
    };
  };

std::string make_simple_config(const string& method) {
  pfi::text::json::json js(new pfi::text::json::json_object());
  js["method"] = pfi::text::json::json(new pfi::text::json::json_string(method));  
  jubatus::fv_converter::converter_config config;
  jubatus::fv_converter::num_rule rule = { "*", "num" };
  config.num_rules.push_back(rule);
  std::stringstream conv;
  conv << config_to_string(config);
  pfi::text::json::json jsc;
  conv >> jsc;
  js["converter"] = jsc;

  std::stringstream ret;
  ret << pfi::text::json::pretty(js);

  return ret.str();
}

TEST_F(recommender_test, get_status){
  jubatus::client::recommender cli("localhost", PORT, 10);
  map<string,map<string,string> > status = cli.get_status(NAME);
  EXPECT_EQ(status.size(), 1u);
  for(map<string,map<string,string> >::const_iterator it = status.begin();
      it != status.end(); ++it){
    EXPECT_GE(it->second.size(), 8u);
  }
}

TEST_F(recommender_test, small) {

  jubatus::client::recommender c("localhost", PORT, 10);
  
  string conf = make_simple_config("lsh");
  c.set_config(NAME, conf);

  jubatus::datum d;
  d.num_values.push_back(make_pair("f1", 1.0));
  c.update_row(NAME, "key", d);
  c.clear_row(NAME, "key");
  c.update_row(NAME, "key", d);

  jubatus::datum d2 = c.complete_row_from_data(NAME, d);
  jubatus::datum d3 = c.complete_row_from_id(NAME, "key");
  //  cout << res.size() << endl;

  c.save(NAME, "name");
  c.load(NAME, "name");
}

TEST_F(recommender_test, throws_in_cast_not_configured) {
  jubatus::client::recommender cli("localhost", PORT, 10);
  EXPECT_THROW(cli.get_all_rows(NAME), std::exception);
  EXPECT_THROW(cli.get_config(NAME), std::exception);
  EXPECT_THROW(cli.clear_row(NAME,"k"), std::exception);

  jubatus::datum d;
  d.num_values.push_back(make_pair("f1", 1.0));
  EXPECT_THROW(cli.update_row(NAME, "k", d), std::exception);

  EXPECT_THROW(cli.complete_row_from_id(NAME, "k"), std::exception);
  EXPECT_THROW(cli.complete_row_from_data(NAME, d), std::exception);
  EXPECT_THROW(cli.similar_row_from_id(NAME, "k",1), std::exception);
  EXPECT_THROW(cli.similar_row_from_data(NAME, d, 1), std::exception);
  EXPECT_THROW(cli.decode_row(NAME, "k"), std::exception);
  EXPECT_THROW(cli.clear(NAME), std::exception);
  EXPECT_THROW(cli.similarity(NAME,d,d), std::exception);
  EXPECT_THROW(cli.l2norm(NAME,d), std::exception);
  EXPECT_THROW(cli.get_all_rows(NAME), std::exception);
}

sfv_diff_t make_vec(float v1, float v2, float v3) {
  sfv_diff_t v;
  v.push_back(make_pair("c1", v1));
  v.push_back(make_pair("c2", v2));
  v.push_back(make_pair("c3", v3));
  return v;
}
sfv_diff_t make_vec(const string& c1, const string& c2, const string& c3) {
  sfv_diff_t v;
  v.push_back(make_pair(c1, 1.0));
  v.push_back(make_pair(c2, 1.0));
  v.push_back(make_pair(c3, 1.0));
  return v;
}


template <typename T>
class recommender_random_test : public testing::Test {};

TYPED_TEST_CASE_P(recommender_random_test);

TYPED_TEST_P(recommender_random_test, trivial) {
  TypeParam r;

  r.update_row("r1", make_vec("c1", "c2", "c3"));
  r.update_row("r2", make_vec("c4", "c5", "c6"));

  vector<pair<string, float> > ids;
  r.similar_row(make_vec("c1", "c2", "c3"), ids, 1);
  ASSERT_EQ(1u, ids.size());
  EXPECT_EQ("r1", ids[0].first);
}

TYPED_TEST_P(recommender_random_test, random) {
  TypeParam r;

  // Generate random data from two norma distributions, N1 and N2.
  vector<float> mu1;
  mu1.push_back(1.0);
  mu1.push_back(1.0);
  mu1.push_back(1.0);
  for (size_t i = 0; i < 100; ++i) {
    vector<double> v;
    make_random(mu1, 1.0, 3, v);
    string row_name = "r1_" + lexical_cast<string>(i);
    r.update_row(row_name, make_vec(v[0], v[1], v[2]));
  }

  vector<float> mu2;
  mu2.push_back(-1.0);
  mu2.push_back(-1.0);
  mu2.push_back(-1.0);
  for (size_t i = 0; i < 100; ++i) {
    vector<double> v;
    make_random(mu2, 1.0, 3, v);
    string row_name = "r2_" + lexical_cast<string>(i);
    r.update_row(row_name, make_vec(v[0], v[1], v[2]));
  }

  // Then, recommend to mean of N1
  vector<pair<string, float> > ids;
  r.similar_row(make_vec(1.0, 1.0, 1.0), ids, 10);
  ASSERT_EQ(10u, ids.size());
  size_t correct = 0;
  for (size_t i = 0; i < ids.size(); ++i) {
    if (ids[i].first[1] == '1')
      ++correct;
  }
  EXPECT_GT(correct, 5u);

  // save the recommender
  stringstream oss;
  r.save(oss);
  TypeParam r2;  
  r2.load(oss);

  // Run the same test
  ids.clear();
  r2.similar_row(make_vec(1.0, 1.0, 1.0), ids, 10);
  ASSERT_EQ(10u, ids.size());
  correct = 0;
  for (size_t i = 0; i < ids.size(); ++i) {
    if (ids[i].first[1] == '1')
      ++correct;
  }
  EXPECT_GT(correct, 5u);
}

}
