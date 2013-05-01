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
#include <gtest/gtest.h>
#include "datum.hpp"
#include "exception.hpp"
#include "libsvm_converter.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

TEST(libsvm_converter, trivial) {
  std::string line = "-1 1:100 2:-1.5";

  std::string label;
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
  std::string line = "1 1,100";
  std::string label;
  datum d;

  ASSERT_THROW(libsvm_converter::convert(line, d, label), converter_exception);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
