// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <map>
#include <string>
#include <gtest/gtest.h>
#include "jubatus/util/lang/scoped_ptr.h"
#include "jubatus/core/fv_converter/../common/type.hpp"
#include "dynamic_binary_feature.hpp"

namespace jubatus {
namespace server {
namespace fv_converter {

TEST(dynamic_binary_feature, trivial) {
  std::map<std::string, std::string> params;

  {
    dynamic_binary_feature f(
        LIBBINARY_FEATURE_SAMPLE,
        "create",
        params);
    jubatus::core::common::sfv_t fv;
    f.add_feature("/path", "value", fv);

    ASSERT_EQ(1u, fv.size());
    EXPECT_EQ("/path", fv[0].first);
    // This plugin returns length of a given value.
    EXPECT_EQ(5., fv[0].second);
  }
}

}  // namespace fv_converter
}  // namespace server
}  // namespace jubatus
