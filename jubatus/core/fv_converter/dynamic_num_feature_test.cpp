// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <pficommon/lang/scoped_ptr.h>
#include "dynamic_num_feature.hpp"
#include "num_feature.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(dynamic_num_feature, trivial) {
  std::map<std::string, std::string> params;

  {
    dynamic_num_feature f(LIBNUM_FEATURE_SAMPLE,
        "create",
        params);
    sfv_t fv;
    f.add_feature("/path", 1, fv);

    ASSERT_EQ(1u, fv.size());
    EXPECT_EQ("/path", fv[0].first);
    EXPECT_EQ(2., fv[0].second);
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
