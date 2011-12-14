// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <gtest/gtest.h>
#include <map>
#include <string>
#include <pficommon/lang/scoped_ptr.h>

#include "num_feature.hpp"
#include "dynamic_num_feature.hpp"

using namespace std;
using namespace jubatus;
using namespace pfi::lang;

TEST(dynamic_num_feature, trivial) {
  map<string, string> params;

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
