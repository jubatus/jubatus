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
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include "jubatus/util/lang/cast.h"

#include "classifier_factory.hpp"
#include "classifier.hpp"
#include "../storage/local_storage.hpp"
#include "../common/exception.hpp"
#include "../common/jsonconfig.hpp"
#include "../unlearner/lru_unlearner.hpp"
#include "classifier_test_util.hpp"

using std::pair;
using std::string;
using std::vector;
using jubatus::util::text::json::to_json;
using jubatus::util::lang::lexical_cast;
using jubatus::core::storage::local_storage;

namespace jubatus {
namespace core {
namespace classifier {

template<typename T>
class classifier_test : public testing::Test {
};

TYPED_TEST_CASE_P(classifier_test);

TYPED_TEST_P(classifier_test, trivial) {
  classifier_base::storage_ptr s(new local_storage);
  TypeParam p(s);
  ASSERT_NE(p.name(), "");
  common::sfv_t fv;
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
  classifier_base::storage_ptr s(new local_storage);
  TypeParam p(s);
  common::sfv_t fv;
  fv.push_back(make_pair(string("f1"), 0.0));
  p.train(fv, string("label1"));
  fv.push_back(make_pair(string("f2"), 1.0));
  p.train(fv, string("label2"));
  classify_result scores;
  p.classify_with_scores(fv, scores);
  ASSERT_EQ(2u, scores.size());
}

common::sfv_t convert(vector<double>& v) {
  common::sfv_t fv;
  for (size_t i = 0; i < v.size(); ++i) {
    string f = "f" + lexical_cast<string>(i);
    fv.push_back(make_pair(f, v[i]));
  }
  return fv;
}

TYPED_TEST_P(classifier_test, random) {
  jubatus::util::math::random::mtrand rand(0);
  classifier_base::storage_ptr s(new local_storage);
  TypeParam p(s);

  srand(0);
  for (size_t i = 0; i < 1000; ++i) {
    pair<string, vector<double> > d = gen_random_data(rand);
    p.train(convert(d.second), d.first);
  }

  size_t correct = 0;
  for (size_t i = 0; i < 100; ++i) {
    pair<string, vector<double> > d = gen_random_data(rand);
    if (d.first == p.classify(convert(d.second))) {
      ++correct;
    }
  }
  EXPECT_GT(correct, 95u);
}

TYPED_TEST_P(classifier_test, random3) {
  jubatus::util::math::random::mtrand rand(0);
  classifier_base::storage_ptr s(new local_storage);
  TypeParam p(s);

  srand(0);
  for (size_t i = 0; i < 1000; ++i) {
    pair<string, vector<double> > d = gen_random_data3(rand);
    p.train(convert(d.second), d.first);
  }

  size_t correct = 0;
  for (size_t i = 0; i < 100; ++i) {
    pair<string, vector<double> > d = gen_random_data3(rand);
    if (d.first == p.classify(convert(d.second))) {
      ++correct;
    }
  }
  EXPECT_GT(correct, 95u);
}

TYPED_TEST_P(classifier_test, delete_class) {
  local_storage s;
  TypeParam p(&s);

  common::sfv_t fv;
  fv.push_back(std::make_pair("f1", 1.f));
  p.train(fv, "A");

  fv.clear();
  fv.push_back(std::make_pair("f1", 1.f));
  fv.push_back(std::make_pair("f2", 1.f));
  p.train(fv, "B");

  fv.clear();
  fv.push_back(std::make_pair("f3", 1.f));
  p.train(fv, "C");

  p.delete_class("B");

  fv.clear();
  fv.push_back(std::make_pair("f1", 1.f));
  fv.push_back(std::make_pair("f2", 1.f));
  EXPECT_EQ("A", p.classify(fv));
}

TYPED_TEST_P(classifier_test, unlearning) {
  local_storage s;
  TypeParam p(&s);

  unlearner::lru_unlearner::config config;
  config.max_size = 2;
  jubatus::util::lang::shared_ptr<unlearner::unlearner_base> unlearner(
      new unlearner::lru_unlearner(config));
  p.set_label_unlearner(unlearner);

  common::sfv_t fv;
  fv.push_back(std::make_pair("f1", 10.f));
  p.train(fv, "A");

  fv.clear();
  fv.push_back(std::make_pair("f2", 1.f));
  p.train(fv, "B");

  fv.clear();
  fv.push_back(std::make_pair("f1", 1.f));
  p.train(fv, "C");

  EXPECT_EQ("C", p.classify(fv));
}

REGISTER_TYPED_TEST_CASE_P(
    classifier_test,
    trivial,
    sfv_err,
    random,
    random3,
    delete_class,
    unlearning);

typedef testing::Types<
  perceptron, passive_aggressive, passive_aggressive_1, passive_aggressive_2,
  confidence_weighted, arow, normal_herd>
  classifier_types;

INSTANTIATE_TYPED_TEST_CASE_P(cl, classifier_test, classifier_types);

TEST(classifier_factory, exception) {
  common::jsonconfig::config param(to_json(classifier_config()));
  classifier_base::storage_ptr p(new local_storage);
  ASSERT_THROW(classifier_factory::create_classifier("pa", param, p),
      common::unsupported_method);
  ASSERT_THROW(classifier_factory::create_classifier("", param, p),
      common::unsupported_method);
  ASSERT_THROW(classifier_factory::create_classifier("saitama", param, p),
      common::unsupported_method);
}

}  // namespace classifier
}  // namespace core
}  // namespace jubatus
