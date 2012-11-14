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

#include <gtest/gtest.h>

#include <string>
#include <map>
#include <pficommon/lang/scoped_ptr.h>

#include "num_feature_factory.hpp"
#include "num_feature.hpp"
#include "exception.hpp"
#include "../common/type.hpp"

using namespace std;
using namespace jubatus;
using namespace jubatus::fv_converter;
using namespace pfi::lang;

TEST(num_feature_factory, trivial) {
  num_feature_factory f;
  map<string, string> param;
  ASSERT_THROW(f.create("hoge", param), converter_exception);
}

TEST(num_feature_factory, dynamic) {
  num_feature_factory f;
  map<string, string> param;

  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["path"] = "unknown_file_name";
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["path"] = LIBNUM_FEATURE_SAMPLE;
  ASSERT_THROW(f.create("dynamic", param), converter_exception);

  param["function"] = "create";
  scoped_ptr<num_feature> s(f.create("dynamic", param));

  sfv_t fv;
  s->add_feature("/path", 1, fv);

  ASSERT_EQ(1u, fv.size());
  EXPECT_EQ("/path", fv[0].first);
  EXPECT_EQ(2., fv[0].second);

}
