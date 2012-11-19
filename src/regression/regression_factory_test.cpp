#include <gtest/gtest.h>
#include <stdexcept>
#include <pficommon/lang/scoped_ptr.h>

#include "regression_factory.hpp"
#include "regression.hpp"
#include "../storage/local_storage.hpp"
#include "../common/exception.hpp"

namespace jubatus {
namespace regression {

TEST(regression_factory, trivial) {
  storage::local_storage s;
  pfi::lang::scoped_ptr<regression_base> r(create_regression("PA", &s));
  EXPECT_EQ(typeid(*r), typeid(regression::PA&));
}

TEST(regression_factory, unknown) {
  storage::local_storage s;
  ASSERT_THROW(create_regression("unknown_regression", &s), unsupported_method);
}

}
}
