#include <gtest/gtest.h>
#include "server_util.hpp"

#include "../fv_converter/exception.hpp"

namespace jubatus {
namespace framework {

TEST(make_fv_converter, empty) {
  EXPECT_THROW(jubatus::framework::make_fv_converter(""), fv_converter::converter_exception);
}

}
}
