#include <gtest/gtest.h>
#include "regression.hpp"
#include "../storage/local_storage.hpp"
#include "regression_test_util.hpp"
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

//FIXME same as classifier_test.cpp
sfv_t convert(vector<double>& v) {
  sfv_t fv;
  for (size_t i = 0; i < v.size(); ++i) {
    string f = "f" + lexical_cast<string>(i);
    fv.push_back(make_pair(f, v[i]));
  }
  return fv;
}

void random_test(regression::regression_base& p, float x, float y, size_t dim) {
  pfi::math::random::mtrand rand(0);
  // learn with 1000 random data
  for (size_t i = 0; i < 1000; ++i) {
    std::pair<float, std::vector<double> > tfv = gen_random_data(x , x, dim);
    p.train(convert(tfv.second), tfv.first);
  }

  size_t ok = 0;
  for (size_t i = 0; i < 100; ++i) {
    std::pair<float, std::vector<double> > tfv = gen_random_data(x , x, dim);
  if (fabs(p.estimate(convert(tfv.second)) - tfv.first) < 2.0 * y * x)
      ++ok;
  }
  EXPECT_GT(ok, 90u);
}

TYPED_TEST_P(regression_test, random) {
  {
    local_storage s;
    TypeParam p(&s);
    random_test(p, 1, 1, 3);
  }
  {
    local_storage s;
    TypeParam p(&s);
    random_test(p, 1, 100, 5);
  }
  {
    local_storage s;
    TypeParam p(&s);
    random_test(p, 10000, 1, 10);
  }
}



REGISTER_TYPED_TEST_CASE_P(
    regression_test,
    trivial, random);

typedef testing::Types<regression::PA> regression_types;

INSTANTIATE_TYPED_TEST_CASE_P(reg, regression_test, regression_types);

}
