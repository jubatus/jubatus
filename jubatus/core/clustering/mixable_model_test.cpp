// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <string>
#include <gtest/gtest.h>
#include "jubatus/util/lang/scoped_ptr.h"
#include "clustering_config.hpp"
#include "clustering.hpp"
#include "testutil.hpp"

namespace jubatus {
namespace core {
namespace clustering {

class mixable_model_test : public ::testing::Test {
  protected:
  const std::string name_;
  mixable_model_test() : name_("name") {
    clustering_config cfg;
    cfg.k = 3;
    cfg.compressor_method = "compressive_kmeans";
    cfg.bucket_size = 10000;
    cfg.compressed_bucket_size = 400;

    model_.reset(new clustering(name_, "kmeans", cfg));
    storage_ = model_->get_storage();
  }

  jubatus::util::lang::scoped_ptr<clustering> model_;
  jubatus::util::lang::shared_ptr<storage> storage_;
};

TEST_F(mixable_model_test, get_diff) {
  static const size_t N = 200;
  static const size_t D = 2;
  model_->push(get_points(N, D));
  diff_t df;
  storage_->get_diff(df);
  ASSERT_EQ(df.size(), (size_t)1);
  ASSERT_EQ(df[0].first, name_);
  ASSERT_EQ(df[0].second.size(), N);
}

TEST_F(mixable_model_test, reduce) {
  static const size_t N = 200;
  static const size_t D = 2;
  model_->push(get_points(N, D));
  diff_t df;
  storage_->get_diff(df);
  diff_t df2;
  storage_->mix(df, df2);
  ASSERT_EQ(df2.size(), (size_t)1);
  ASSERT_EQ(df2[0].first, name_);
  ASSERT_EQ(df2[0].second.size(), N);

  storage_->get_diff(df2);
  storage_->mix(df, df2);
  ASSERT_EQ(df2.size(), (size_t)1);
  ASSERT_EQ(df2[0].first, name_);
  ASSERT_EQ(df2[0].second.size(), 2*N);

  storage_->get_diff(df2);
  df2[0].first += "2";
  storage_->mix(df, df2);
  ASSERT_EQ(df2.size(), (size_t)2);
  ASSERT_EQ(df2[0].first, name_);
  ASSERT_EQ(df2[0].second.size(), N);
  ASSERT_EQ(df2[1].first, name_+"2");
  ASSERT_EQ(df2[1].second.size(), N);
}

TEST_F(mixable_model_test, put_diff) {
  static const size_t N = 200;
  static const size_t D = 2;
  model_->push(get_points(N, D));
  diff_t df, df2;
  storage_->get_diff(df);
  storage_->get_diff(df2);
  df2[0].first = "name2";
  storage_->mix(df, df2);
  storage_->set_mixed_and_clear_diff(df2);

  wplist coreset = model_->get_coreset();
  ASSERT_EQ(coreset.size(), 2*N);
  storage_->get_diff(df);
  ASSERT_EQ(df.size(), (size_t)1);
  ASSERT_EQ(df[0].second.size(), N);
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
