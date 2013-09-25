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

#include <complex>
#include <string>
#include <valarray>
#include <gtest/gtest.h>
#include "num_feature_impl.hpp"

namespace jubatus {
namespace fv_converter {

TEST(num_feature_impl, num_concat_feature) {
  const std::string key("user/name$Taro Yamada@str#bin/bin");
  std::vector<std::pair<uint64_t, double> > value;
  value.push_back(std::make_pair(0, 1.0));
  value.push_back(std::make_pair(1, 1.0));
  sfv_t ret_fv;
  num_concat_feature concat;
  concat.add_feature(key, value, ret_fv);
  ASSERT_EQ("user/name$Taro Yamada@str#bin/bin_concat$2", ret_fv[0].first);
  EXPECT_EQ(1.0, ret_fv[0].second);
}

TEST(num_feature_impl, num_average_feature) {
  const double test[] = {1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  const std::string key("test_key");
  std::vector<std::pair<uint64_t, double> > value;
  sfv_t ret_fv;
  for (uint64_t i = 0; i < 8; ++i) {
    value.push_back(std::make_pair(i, test[i]));
  }
  num_average_feature average;
  average.add_feature(key, value, ret_fv);
  EXPECT_EQ(0.25, ret_fv[0].second);
}

TEST(num_feature_impl, num_stddev_feature) {
  const double test[] = {1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  const std::string key("test_key");
  std::vector<std::pair<uint64_t, double> > value;
  sfv_t ret_fv;
  for (uint64_t i = 0; i < 8; ++i) {
    value.push_back(std::make_pair(i, test[i]));
  }
  num_stddev_feature stddev;
  stddev.add_feature(key, value, ret_fv);
  EXPECT_NEAR(0.433012, ret_fv[0].second, 1.0e-6);
}

TEST(num_feature_impl, num_fft_feature) {
  const double test[] = {0.0, 1.0, 2.0, 0.0, 0.0, 2.0, 1.0};
  const std::string key("test_key");
  std::vector<std::pair<uint64_t, double> > value;
  sfv_t ret_fv;
  for (uint64_t i = 0; i < 8; ++i) {
    value.push_back(std::make_pair(i, test[i]));
  }
  num_fft_feature fft;
  fft.add_feature(key, value, ret_fv);
  EXPECT_NEAR(-0.707107, ret_fv[2].second, 1.0e-6);
}

TEST(num_feature_impl, num_diff_feature) {
  const double test[] = {0.0, 0.0, 1.0};
  const std::string key("test_key");
  std::vector<std::pair<uint64_t, double> > value;
  sfv_t ret_fv;
  for (uint64_t i = 0; i < 3; ++i) {
    value.push_back(std::make_pair(i, test[i]));
  }
  num_diff_feature diff;
  diff.add_feature(key, value, ret_fv);
  EXPECT_EQ(1.0 - 0.0, ret_fv[0].second);
}

}  // namespace fv_converter
}  // namespace jubatus
