#include <gtest/gtest.h>
#include "stat.hpp"
#include <pficommon/math/random.h>

using namespace std;
using namespace pfi::lang;

namespace jubatus {

template <typename T>
class stat_test : public testing::Test {};

TYPED_TEST_CASE_P(stat_test);

TYPED_TEST_P(stat_test, trivial) {
  TypeParam p(10);
  p.push("test", 1.0);
  p.push("test", 2.0);
  p.push("test", 3.0);

  EXPECT_TRUE(p.sum("test") == 6.0);
  EXPECT_TRUE(p.max("test") == 3.0);
  EXPECT_TRUE(p.min("test") == 1.0);
  EXPECT_NEAR(p.moment("test", 0 , 0.0) , 1.0, 0.1);
  EXPECT_NEAR(p.moment("test", 1 , 0.0) , 2.0, 0.1);
  EXPECT_NEAR(p.moment("test", 2 , 0.0) , 4.67, 0.1);
  EXPECT_NEAR(p.moment("test", 2 , 2.0) , 0.67, 0.1);
  EXPECT_NEAR(p.moment("test", 3 , 0.0) , 12.0, 0.1);
  EXPECT_NEAR(p.stddev("test"), 0.82, 0.1);

}


REGISTER_TYPED_TEST_CASE_P(
    stat_test,
    trivial);

typedef testing::Types<stat::stat> stat_types;

INSTANTIATE_TYPED_TEST_CASE_P(stt, stat_test, stat_types);

}
