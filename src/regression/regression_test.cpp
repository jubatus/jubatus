#include <gtest/gtest.h>
#include "regression.hpp"
#include "../storage/local_storage.hpp"
#include <pficommon/math/random.h>

using namespace std;
using namespace jubatus::storage;
using namespace pfi::lang;

namespace jubatus {

template <typename T>
class regression_test : public testing::Test {};

TYPED_TEST_CASE_P(regression_test);

TYPED_TEST_P(regression_test, trivial) {
  local_storage s;
  TypeParam p(&s);
  sfv_t fv;
  fv.push_back(make_pair(string("f1"), 1.0));
  p.train(fv, 10);
  
  fv.clear();
  fv.push_back(make_pair(string("f2"), 1.0));
  p.train(fv, -10);

  fv.clear();
  fv.push_back(make_pair("f1", 2.0));
  EXPECT_TRUE(p.estimate(fv) > 0.0);
}

void random_test(regression_base& p, float x, float y) {
  pfi::math::random::mtrand rand(0);
  // learn with 1000 random data
  for (size_t i = 0; i < 1000; ++i) {
    sfv_t fv;
    float f1 = rand.next_gaussian(x, x);
    float f2 = rand.next_gaussian(x, x);
    float f3 = rand.next_gaussian(x, x);
    float value = y * (f1 * 5 + f2 * 3 + f3 * 2 + rand.next_gaussian() * x);
    
    fv.push_back(make_pair("f1", f1));
    fv.push_back(make_pair("f2", f2));
    fv.push_back(make_pair("f3", f3));
    p.train(fv, value);
  }

  size_t ok = 0;
  for (size_t i = 0; i < 100; ++i) {
    sfv_t fv;
    float f1 = rand.next_gaussian(x, x);
    float f2 = rand.next_gaussian(x, x);
    float f3 = rand.next_gaussian(x, x);
    float value = y * (f1 * 5 + f2 * 3 + f3 * 2);

    fv.push_back(make_pair("f1", f1));
    fv.push_back(make_pair("f2", f2));
    fv.push_back(make_pair("f3", f3));
    if (fabs(p.estimate(fv) - value) < 2.0 * y * x)
      ++ok;
  }
  EXPECT_GT(ok, 90u);
}

TYPED_TEST_P(regression_test, random) {
  {
    local_storage s;
    TypeParam p(&s);
    random_test(p, 1, 1);
  }
  {
    local_storage s;
    TypeParam p(&s);
    random_test(p, 1, 100);
  }
  {
    local_storage s;
    TypeParam p(&s);
    random_test(p, 10000, 1);
  }
}



REGISTER_TYPED_TEST_CASE_P(
    regression_test,
    trivial, random);

typedef testing::Types<regression::PA> regression_types;

INSTANTIATE_TYPED_TEST_CASE_P(reg, regression_test, regression_types);

}
