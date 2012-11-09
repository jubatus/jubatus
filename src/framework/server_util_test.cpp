#include <gtest/gtest.h>
#include "server_util.hpp"

#include "../fv_converter/exception.hpp"

namespace jubatus {
namespace framework {

TEST(make_fv_converter, empty) {
  EXPECT_THROW(jubatus::framework::make_fv_converter(""), fv_converter::converter_exception);
}

TEST(make_fv_converter, invalid_config_json) {
  EXPECT_THROW(jubatus::framework::make_fv_converter("{"), fv_converter::converter_exception);
}

TEST(make_fv_converter, config_json_parse_error) {
  EXPECT_THROW(jubatus::framework::make_fv_converter("AA"), fv_converter::converter_exception);
}

TEST(make_fv_converter, config_cast_error) {
  EXPECT_THROW(jubatus::framework::make_fv_converter("{}"), fv_converter::converter_exception);
}

}
}
