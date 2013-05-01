// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <gtest/gtest.h>

#include "feature_hasher.hpp"
#include "exception.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(feature_hasher, trivial) {
  feature_hasher h(100);
  sfv_t fv;
  fv.push_back(std::make_pair("f1", 1.0));
  fv.push_back(std::make_pair("f2", 2.0));

  h.hash_feature_keys(fv);

  ASSERT_EQ(2u, fv.size());
  EXPECT_NE("f1", fv[0].first);
  EXPECT_EQ(1.0, fv[0].second);
  EXPECT_NE("f2", fv[1].first);
  EXPECT_EQ(2.0, fv[1].second);
}

TEST(feature_hasher, zero) {
  EXPECT_THROW(feature_hasher(0), converter_exception);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
