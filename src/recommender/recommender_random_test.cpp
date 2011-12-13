#include <gtest/gtest.h>

#include <sstream>
#include <pficommon/lang/cast.h>
#include "recommender.hpp"
#include "../classifier/classifier_test_util.hpp"

namespace jubatus {
namespace recommender {

using namespace std;
using namespace pfi::lang;

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

TYPED_TEST_P(recommender_random_test, save_load) {
  TypeParam r;

  // Generate random data
  vector<float> mu(3);
  for (size_t i = 0; i < 100; ++i) {
    vector<double> v;
    make_random(mu, 1.0, 3, v);
    string row_name = "r1_" + lexical_cast<string>(i);
    r.update_row(row_name, make_vec(v[0], v[1], v[2]));
  }

  vector<pair<string, float> > ids_before;
  r.recommender_base::similar_row(string("r1_0"), ids_before, 10);
  sfv_t comp_before;
  r.complete_row("r1_0", comp_before);

  stringstream ss;
  r.save(ss);
  TypeParam r2;
  r2.load(ss);

  vector<pair<string, float> > ids_after;
  r2.recommender_base::similar_row(string("r1_0"), ids_after, 10);
  sfv_t comp_after;
  r2.complete_row("r1_0", comp_after);

  vector<string> row_ids;
  r2.get_all_row_ids(row_ids);
  EXPECT_EQ(100u, row_ids.size())
      << "You may forget to call recommener_base::save_base in save method.";

  EXPECT_TRUE(ids_before == ids_after);
  EXPECT_TRUE(comp_before == comp_after);
}

REGISTER_TYPED_TEST_CASE_P(recommender_random_test,
                           trivial, random, save_load);

typedef testing::Types<inverted_index, lsh, minhash> recommender_types;

INSTANTIATE_TYPED_TEST_CASE_P(rt, recommender_random_test, recommender_types);

}
}
