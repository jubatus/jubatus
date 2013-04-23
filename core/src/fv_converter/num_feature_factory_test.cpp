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

#include <string>
#include <map>
#include <gtest/gtest.h>
#include <pficommon/lang/scoped_ptr.h>
#include "../common/type.hpp"
#include "exception.hpp"
#include "num_feature.hpp"
#include "num_feature_factory.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(num_feature_factory, trivial) {
  num_feature_factory f;
  std::map<std::string, std::string> param;
  ASSERT_THROW(f.create("hoge", param), converter_exception);
}

TEST(num_feature_factory, dynamic) {
  num_feature_factory f;
  std::map<std::string, std::string> param;

  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["path"] = "unknown_file_name";
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["path"] = LIBNUM_FEATURE_SAMPLE;
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["function"] = "create";
  pfi::lang::scoped_ptr<num_feature> s(f.create("dynamic", param));

  sfv_t fv;
  s->add_feature("/path", 1, fv);

  ASSERT_EQ(1u, fv.size());
  EXPECT_EQ("/path", fv[0].first);
  EXPECT_EQ(2., fv[0].second);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
