// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2017 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include <string>
#include <gtest/gtest.h>

#include "server_base.hpp"

namespace jubatus {
namespace server {
namespace framework {

TEST(validate_model_id, simple) {
    EXPECT_TRUE(validate_model_id("valid_model_id"));
    EXPECT_TRUE(validate_model_id(
        "valid_chars !\"#$%&'()*+,-."
        "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]"
        "^_`abcdefghijklmnopqrstuvwxyz{|}~"));
    EXPECT_FALSE(validate_model_id("invalid/../model/id/"));

    std::string nullstr("null");
    nullstr.push_back(0);
    EXPECT_FALSE(validate_model_id(nullstr));
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
