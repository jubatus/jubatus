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
#include "cht.hpp"

using std::string;

namespace jubatus {
namespace server {
namespace common {

TEST(cht, make_hash) {
  string hash = make_hash("hage");
  string hash2 = make_hash("hage");
  string hash3 = make_hash("hoge");
  ASSERT_EQ(hash, hash2);
  ASSERT_NE(hash, hash3);
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
