#include <gtest/gtest.h>
#include "regression.hpp"
#include "../storage/local_storage.hpp"
#include "../classifier/classifier_test_util.hpp"

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

TYPED_TEST_P(regression_test, random) {
  local_storage s;
  TypeParam p(&s);

  // learn with 1000 random data
  for (size_t i = 0; i < 1000; ++i) {
    sfv_t fv;
    float f1 = rand_normal();
    float f2 = rand_normal();
    float f3 = rand_normal();
    float value = f1 * 5 + f2 * 3 + f3 * 2 + rand_normal();
    
    fv.push_back(make_pair("f1", f1));
    fv.push_back(make_pair("f2", f2));
    fv.push_back(make_pair("f3", f3));
    p.train(fv, value);
  }

  size_t ok = 0;
  for (size_t i = 0; i < 100; ++i) {
    sfv_t fv;
    float f1 = rand_normal();
    float f2 = rand_normal();
    float f3 = rand_normal();
    float value = f1 * 5 + f2 * 3 + f3 * 2;

    fv.push_back(make_pair("f1", f1));
    fv.push_back(make_pair("f2", f2));
    fv.push_back(make_pair("f3", f3));
    if (fabs(p.estimate(fv) - value) < 2.0)
      ++ok;
  }
  EXPECT_GT(ok, 95u);
}

REGISTER_TYPED_TEST_CASE_P(
    regression_test,
    trivial, random);

typedef testing::Types<online_svr> regression_types;

INSTANTIATE_TYPED_TEST_CASE_P(reg, regression_test, regression_types);

}
