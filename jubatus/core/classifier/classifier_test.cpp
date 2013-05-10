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

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <pficommon/lang/cast.h>

#include "classifier_factory.hpp"
#include "classifier.hpp"
#include "../storage/local_storage.hpp"
#include "../common/exception.hpp"
#include "../common/jsonconfig.hpp"
#include "classifier_test_util.hpp"

using std::pair;
using std::string;
using std::vector;
using pfi::text::json::to_json;
using pfi::lang::lexical_cast;
using jubatus::core::storage::local_storage;

namespace jubatus {
namespace core {
namespace classifier {

template<typename T>
class classifier_test : public testing::Test {
};

TYPED_TEST_CASE_P(classifier_test);

TYPED_TEST_P(classifier_test, trivial) {
  local_storage s;
  TypeParam p(&s);
  ASSERT_NE(p.name(), "");
  sfv_t fv;
  fv.push_back(make_pair(string("f1"), 1.0));
  p.train(fv, string("label1"));
  fv.push_back(make_pair(string("f2"), 1.0));
  p.train(fv, string("label2"));
  classify_result scores;
  p.classify_with_scores(fv, scores);
  ASSERT_EQ(2u, scores.size());

  p.clear();

  p.classify_with_scores(fv, scores);
  ASSERT_EQ(0u, scores.size());
}

TYPED_TEST_P(classifier_test, sfv_err) {
  local_storage s;
  TypeParam p(&s);
  sfv_t fv;
  fv.push_back(make_pair(string("f1"), 0.0));
  p.train(fv, string("label1"));
  fv.push_back(make_pair(string("f2"), 1.0));
  p.train(fv, string("label2"));
  classify_result scores;
  p.classify_with_scores(fv, scores);
  ASSERT_EQ(1u, scores.size());
  // TODO(kuenishi) why not
  // ASSERT_EQ(2u, scores.size());
}

sfv_t convert(vector<double>& v) {
  sfv_t fv;
  for (size_t i = 0; i < v.size(); ++i) {
    string f = "f" + lexical_cast<string>(i);
    fv.push_back(make_pair(f, v[i]));
  }
  return fv;
}

TYPED_TEST_P(classifier_test, random) {
  local_storage s;
  TypeParam p(&s);

  srand(0);
  for (size_t i = 0; i < 1000; ++i) {
    pair<string, vector<double> > d = gen_random_data();
    p.train(convert(d.second), d.first);
  }

  size_t correct = 0;
  for (size_t i = 0; i < 100; ++i) {
    pair<string, vector<double> > d = gen_random_data();
    if (d.first == p.classify(convert(d.second))) {
      ++correct;
    }
  }
  EXPECT_GT(correct, 95u);
}

TYPED_TEST_P(classifier_test, random3) {
  local_storage s;
  TypeParam p(&s);

  srand(0);
  for (size_t i = 0; i < 1000; ++i) {
    pair<string, vector<double> > d = gen_random_data3();
    p.train(convert(d.second), d.first);
  }

  size_t correct = 0;
  for (size_t i = 0; i < 100; ++i) {
    pair<string, vector<double> > d = gen_random_data3();
    if (d.first == p.classify(convert(d.second))) {
      ++correct;
    }
  }
  EXPECT_GT(correct, 95u);
}

REGISTER_TYPED_TEST_CASE_P(
    classifier_test,
    trivial,
    sfv_err,
    random,
    random3);

typedef testing::Types<
  perceptron, passive_aggressive, passive_aggressive_1, passive_aggressive_2,
  confidence_weighted, arow, normal_herd>
  classifier_types;

INSTANTIATE_TYPED_TEST_CASE_P(cl, classifier_test, classifier_types);

TEST(classifier_factory, exception) {
  jsonconfig::config param(to_json(classifier_config()));
  local_storage* p = new local_storage;
  ASSERT_THROW(classifier_factory::create_classifier("pa", param, p),
      unsupported_method);
  ASSERT_THROW(classifier_factory::create_classifier("", param, p),
      unsupported_method);
  ASSERT_THROW(classifier_factory::create_classifier("saitama", param, p),
      unsupported_method);
  delete p;
}

}  // namespace classifier
}  // namespace core
}  // namespace jubatus
