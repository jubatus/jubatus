// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <string>
#include <utility>

#include <gtest/gtest.h>
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/util/data/serialization.h"
#include "jubatus/util/data/serialization/unordered_map.h"
#include "mixable_weight_manager.hpp"

using std::make_pair;
using std::stringstream;
using std::sort;
using jubatus::util::lang::shared_ptr;
using jubatus::core::fv_converter::keyword_weights;
using jubatus::core::fv_converter::datum;

namespace jubatus {
namespace core {
namespace fv_converter {

class versioned_weight_diff_test : public ::testing::Test {
 public:
  void SetUp() {
    {
      common::sfv_t fv;
      fv.push_back(make_pair("a", 1));
      kw1.update_document_frequency(fv);
    }
    {
      common::sfv_t fv;
      fv.push_back(make_pair("b", 1));
      kw1.update_document_frequency(fv);
      kw1.update_document_frequency(fv);
    }
    {
      common::sfv_t fv;
      fv.push_back(make_pair("b", 1));
      kw2.update_document_frequency(fv);
      kw2.update_document_frequency(fv);
      kw2.update_document_frequency(fv);
      kw2.update_document_frequency(fv);
    }
    {
      common::sfv_t fv;
      fv.push_back(make_pair("c", 1));
      kw2.update_document_frequency(fv);
      kw2.update_document_frequency(fv);
      kw2.update_document_frequency(fv);
      kw2.update_document_frequency(fv);
      kw2.update_document_frequency(fv);
      kw2.update_document_frequency(fv);
      kw2.update_document_frequency(fv);
      kw2.update_document_frequency(fv);
    }
  }
  void TearDown() {
    kw1.clear();
    kw2.clear();
  }

  keyword_weights kw1;
  keyword_weights kw2;
};

TEST_F(versioned_weight_diff_test, fixture) {
  ASSERT_EQ(3, kw1.get_document_count());
  ASSERT_EQ(1, kw1.get_document_frequency("a"));
  ASSERT_EQ(2, kw1.get_document_frequency("b"));

  ASSERT_EQ(12, kw2.get_document_count());
  ASSERT_EQ(4, kw2.get_document_frequency("b"));
  ASSERT_EQ(8, kw2.get_document_frequency("c"));
}

TEST_F(versioned_weight_diff_test, merge_success) {
  versioned_weight_diff vw1(kw1);
  versioned_weight_diff vw2(kw2);

  vw1.version_.increment();
  vw2.version_.increment();
  vw1.merge(vw2);
  ASSERT_EQ(15, vw1.weights_.get_document_count());
  ASSERT_EQ(1, vw1.weights_.get_document_frequency("a"));
  ASSERT_EQ(6, vw1.weights_.get_document_frequency("b"));
  ASSERT_EQ(8, vw1.weights_.get_document_frequency("c"));
}

TEST_F(versioned_weight_diff_test, merge_vw1_win) {
  versioned_weight_diff vw1(kw1);
  versioned_weight_diff vw2(kw2);

  vw1.version_.increment();
  vw1.merge(vw2);
  ASSERT_EQ(3, vw1.weights_.get_document_count());
  ASSERT_EQ(1, vw1.weights_.get_document_frequency("a"));
  ASSERT_EQ(2, vw1.weights_.get_document_frequency("b"));
  ASSERT_EQ(0, vw1.weights_.get_document_frequency("c"));
}

TEST_F(versioned_weight_diff_test, merge_vw2_win) {
  versioned_weight_diff vw1(kw1);
  versioned_weight_diff vw2(kw2);

  vw2.version_.increment();
  vw1.merge(vw2);
  ASSERT_EQ(12, vw1.weights_.get_document_count());
  ASSERT_EQ(0, vw1.weights_.get_document_frequency("a"));
  ASSERT_EQ(4, vw1.weights_.get_document_frequency("b"));
  ASSERT_EQ(8, vw1.weights_.get_document_frequency("c"));
}

class mixable_weight_manager_test : public ::testing::Test {
 public:
  void SetUp() {
    shared_ptr<weight_manager> m(new weight_manager);
    mw.reset(new mixable_weight_manager);

    {
      common::sfv_t fv;
      fv.push_back(make_pair("a", 1));
      fv.push_back(make_pair("b", 1));
      m->update_weight(fv);
    }

    mw->set_model(m);
  }
  void TearDown() {
    mw.reset();
  }
  shared_ptr<mixable_weight_manager> mw;
};

TEST_F(mixable_weight_manager_test, fixture) {
  ASSERT_EQ(0, mw->get_version().get_number());
}

TEST_F(mixable_weight_manager_test, get_diff) {
  versioned_weight_diff got = mw->get_diff_impl();
  ASSERT_EQ(0, got.version_.get_number());
  ASSERT_EQ(1, got.weights_.get_document_count());
  ASSERT_EQ(1, got.weights_.get_document_frequency("a"));
  ASSERT_EQ(1, got.weights_.get_document_frequency("b"));
}

TEST_F(mixable_weight_manager_test, put_diff) {
  keyword_weights w;
  {
    common::sfv_t fv;
    fv.push_back(make_pair("a", 1));
    fv.push_back(make_pair("b", 1));
    w.update_document_frequency(fv);
  }

  versioned_weight_diff appender(w);
  mw->put_diff_impl(appender);
  versioned_weight_diff got = mw->get_diff_impl();
  ASSERT_EQ(1, got.version_.get_number());  // should be incremented

  ASSERT_EQ(0, got.weights_.get_document_count());
  ASSERT_EQ(0, got.weights_.get_document_frequency("a"));
  ASSERT_EQ(0, got.weights_.get_document_frequency("b"));

  common::sfv_t result;
  result.push_back(make_pair("a", 2));
  result.push_back(make_pair("b", 3));

  mw->get_model()->get_weight(result);
  ASSERT_EQ(2, result.size());
  ASSERT_EQ("a", result[0].first);
  ASSERT_EQ(2, result[0].second);
  ASSERT_EQ("b", result[1].first);
  ASSERT_EQ(3, result[1].second);
}


}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
