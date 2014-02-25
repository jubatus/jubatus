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

#include <utility>
#include <gtest/gtest.h>
#include "jubatus/util/math/random.h"
#include "stat.hpp"

namespace jubatus {

template<typename T>
class stat_test : public testing::Test {
};

TYPED_TEST_CASE_P(stat_test);

TYPED_TEST_P(stat_test, trivial) {
  TypeParam p(3);
  p.push("test", 1.0);
  p.push("test", 5.0);
  p.push("test", 3.0);

  EXPECT_TRUE(p.sum("test") == 9.0);
  EXPECT_TRUE(p.max("test") == 5.0);
  EXPECT_TRUE(p.min("test") == 1.0);
  EXPECT_NEAR(p.moment("test", 0 , 0.0) , 1.0, 0.1);
  EXPECT_NEAR(p.moment("test", 1 , 0.0) , 3.0, 0.1);
  EXPECT_NEAR(p.moment("test", 2 , 0.0) , 11.67, 0.1);
  EXPECT_NEAR(p.moment("test", 2 , 3.0) , 2.67, 0.1);
  EXPECT_NEAR(p.moment("test", 3 , 0.0) , 51.0, 0.1);
  EXPECT_NEAR(p.stddev("test"), 1.63, 0.01);

  p.push("test", 2.0);
  p.push("test", 4.0);
  EXPECT_TRUE(p.sum("test") == 9.0);
  EXPECT_TRUE(p.max("test") == 4.0);
  EXPECT_TRUE(p.min("test") == 2.0);
  EXPECT_NEAR(p.moment("test", 0 , 0.0) , 1.0, 0.1);
  EXPECT_NEAR(p.moment("test", 1 , 0.0) , 3.0, 0.1);
  EXPECT_NEAR(p.moment("test", 2 , 0.0) , 9.67, 0.1);
  EXPECT_NEAR(p.moment("test", 2 , 3.0) , 0.67, 0.1);
  EXPECT_NEAR(p.moment("test", 3 , 0.0) , 33.0, 0.1);
  EXPECT_NEAR(p.stddev("test"), 0.82, 0.1);

  p.clear();

  p.push("test", 1.0);
  EXPECT_TRUE(p.sum("test") == 1.0);
  EXPECT_TRUE(p.max("test") == 1.0);
  EXPECT_TRUE(p.min("test") == 1.0);
  EXPECT_NEAR(p.moment("test", 0 , 0.0) , 1.0, 0.1);
  EXPECT_NEAR(p.moment("test", 1 , 0.0) , 1.0, 0.1);
  EXPECT_NEAR(p.moment("test", 2 , 0.0) , 1.0, 0.1);
  EXPECT_NEAR(p.moment("test", 2 , 3.0) , 4.0, 0.1);
  EXPECT_NEAR(p.moment("test", 3 , 0.0) , 1.0, 0.1);
  EXPECT_NEAR(p.stddev("test"), 0.0, 0.1);
}

TEST(stat_test, entropy) {
  core::stat::stat p(3);

  p.push("test", 1.0);
  p.push("test", 2.0);
  p.push("test", 3.0);
  EXPECT_TRUE(p.entropy() == 0.0);

  p.push("test1", 1.0);
  p.push("test2", 2.0);
  p.push("test3", 3.0);
  EXPECT_NEAR(p.entropy(), 1.097, 0.01);
}

TEST(stat_test, mixed_entropy) {
  core::stat::stat p(1024);
  p.push("test", 1.0);
  p.push("test", 2.0);
  p.push("test", 3.0);

  double e_d = 3 * std::log(3);
  double e_e = - e_d / 3 + std::log(3);

  std::pair<double, size_t> d;
  p.get_diff(d);
  ASSERT_DOUBLE_EQ(e_d, d.first);
  ASSERT_EQ(3u, d.second);

  p.set_mixed_and_clear_diff(d);

  double bias = d.first / d.second;  // bias to suppress cancellation
  ASSERT_DOUBLE_EQ(e_e + bias, p.entropy() + bias);
  ASSERT_DOUBLE_EQ(p.entropy() + bias, p.entropy() + bias);
}

REGISTER_TYPED_TEST_CASE_P(
    stat_test,
    trivial);

typedef testing::Types<core::stat::stat> stat_types;

INSTANTIATE_TYPED_TEST_CASE_P(stt, stat_test, stat_types);
}  // namespace jubatus
