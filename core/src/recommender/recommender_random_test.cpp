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

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <pficommon/lang/cast.h>

#include "recommender.hpp"
#include "../classifier/classifier_test_util.hpp"

using std::make_pair;
using std::pair;
using std::sort;
using std::string;
using std::stringstream;
using std::vector;
using pfi::lang::lexical_cast;

namespace jubatus {
namespace core {
namespace recommender {

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

template<typename T>
class recommender_random_test : public testing::Test {
};

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

void update_random(recommender_base& r) {
  vector<float> mu(3);
  for (size_t i = 0; i < 100; ++i) {
    vector<double> v;
    make_random(mu, 1.0, 3, v);
    string row_name = "r1_" + lexical_cast<string>(i);
    r.update_row(row_name, make_vec(v[0], v[1], v[2]));
  }
}

void compare_recommenders(recommender_base& r1, recommender_base& r2,
                          bool compare_complete_row = true) {
  // make a query vector
  sfv_t q = make_vec(0.5, 0.3, 1.0);

  // Get result before saving
  vector<pair<string, float> > ids1;
  r1.similar_row(q, ids1, 10);
  sfv_t comp1;
  r1.complete_row("r1_0", comp1);

  // Get result from loaded data
  vector<pair<string, float> > ids2;
  r2.similar_row(q, ids2, 10);
  sfv_t comp2;
  r2.complete_row("r1_0", comp2);

  // Compare two results
  // ID order could not be same if there are score ties.
  // EXPECT_TRUE(ids1 == ids2);
  ASSERT_EQ(ids1.size(), ids2.size());
  for (size_t i = 0; i < ids1.size(); ++i) {
    EXPECT_FLOAT_EQ(ids1[i].second, ids2[i].second);
  }

  if (compare_complete_row)
    EXPECT_TRUE(comp1 == comp2);
}

TYPED_TEST_P(recommender_random_test, save_load) {
  TypeParam r;

  // Generate random data
  update_random(r);

  // save and load
  stringstream ss;
  r.save(ss);
  TypeParam r2;
  r2.load(ss);

  vector<string> row_ids;
  r2.get_all_row_ids(row_ids);
  EXPECT_EQ(100u, row_ids.size());

  compare_recommenders(r, r2);
}

TYPED_TEST_P(recommender_random_test, get_all_row_ids) {
  TypeParam r;
  vector<string> expect_ids;
  for (size_t i = 0; i < 100; ++i) {
    string row_name = "r1_" + lexical_cast<string>(i);
    expect_ids.push_back(row_name);
  }
  update_random(r);

  // Get all ids
  vector<string> ids;
  r.get_all_row_ids(ids);

  // Compare
  sort(expect_ids.begin(), expect_ids.end());
  sort(ids.begin(), ids.end());
  ASSERT_EQ(100u, ids.size());
  ASSERT_TRUE(expect_ids == ids);

  /* TODO this test cannot be passed with current implementation
   // Remove half rows
   for (size_t i = 0; i < 50; ++i) {
   string row_name = "r1_" + lexical_cast<string>(i);
   r.clear_row(row_name);
   }

   // Get rest of ids
   ids.clear();
   r.get_all_row_ids(ids);
   ASSERT_EQ(50u, ids.size());
   expect_ids.erase(expect_ids.begin(), expect_ids.begin() + 50);
   ASSERT_TRUE(expect_ids == ids);
   */
}

TYPED_TEST_P(recommender_random_test, diff) {
  TypeParam r;
  update_random(r);

  string diff;
  r.get_storage()->get_diff(diff);

  TypeParam r2;
  r2.get_storage()->set_mixed_and_clear_diff(diff);

  compare_recommenders(r, r2, false);
}

TYPED_TEST_P(recommender_random_test, mix) {
  TypeParam r1, r2, expect;
  vector<float> mu(10);
  for (size_t i = 0; i < 100; ++i) {
    vector<double> v;
    make_random(mu, 1.0, 3, v);
    sfv_t vec = make_vec(v[0], v[1], v[2]);

    string row = "r_" + lexical_cast<string>(i);
    (i < 50 ? r1 : r2).update_row(row, vec);
    expect.update_row(row, vec);
  }

  string diff1, diff2;
  r1.get_storage()->get_diff(diff1);
  r2.get_storage()->get_diff(diff2);

  r1.get_storage()->mix(diff1, diff2);

  TypeParam mixed;
  mixed.get_storage()->set_mixed_and_clear_diff(diff2);

  compare_recommenders(expect, mixed, false);
}

REGISTER_TYPED_TEST_CASE_P(recommender_random_test,
    trivial, random, save_load, get_all_row_ids,
    diff, mix);

typedef testing::Types<inverted_index, lsh, minhash, euclid_lsh>
  recommender_types;

INSTANTIATE_TYPED_TEST_CASE_P(rt, recommender_random_test, recommender_types);

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
