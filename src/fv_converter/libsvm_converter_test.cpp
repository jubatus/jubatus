// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "datum.hpp"
#include "libsvm_converter.hpp"
#include "exception.hpp"

using namespace std;
using namespace jubatus;
using namespace jubatus::fv_converter;

TEST(libsvm_converter, trivial) {
  string line = "-1 1:100 2:-1.5";

  string label;
  datum d;

  libsvm_converter::convert(line, d, label);

  EXPECT_EQ("-1", label);
  EXPECT_EQ(2u, d.num_values_.size());
  EXPECT_EQ("1", d.num_values_[0].first);
  EXPECT_EQ(100.0, d.num_values_[0].second);
  EXPECT_EQ("2", d.num_values_[1].first);
  EXPECT_EQ(-1.5, d.num_values_[1].second);
}

TEST(libsvm_converter, illegal) {
  string line = "1 1,100";
  string label;
  datum d;

  ASSERT_THROW(libsvm_converter::convert(line, d, label), converter_exception);

}
