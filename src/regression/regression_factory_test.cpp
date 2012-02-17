#include <gtest/gtest.h>
#include <stdexcept>
#include <pficommon/lang/scoped_ptr.h>

#include "regression_factory.hpp"
#include "regression.hpp"
#include "../storage/local_storage.hpp"

namespace jubatus {

TEST(regression_factory, trivial) {
  regression_factory f;
  storage::local_storage s;
  pfi::lang::scoped_ptr<regression_base> r(f.create_regression("PA", &s));
  EXPECT_EQ(typeid(*r), typeid(regression::PA&));
}

TEST(regression_factory, unknown) {
  regression_factory f;
  storage::local_storage s;
  EXPECT_THROW(f.create_regression("unknown_regression", &s), std::runtime_error);
}

}
