// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "storage_type.hpp"

using std::make_pair;
using std::pair;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {
namespace detail {

string concat(string x, string y) {
  return x + "-" + y;
}

TEST(vector_binop, single_applicaton) {
  vector<pair<string, string> > v1;
  vector<pair<string, string> > v2;

  v1.push_back(make_pair(string("A"), string("1")));
  v2.push_back(make_pair(string("A"), string("2")));

  binop(v1, v2, concat);
  ASSERT_EQ(1u, v1.size());
  EXPECT_EQ("1-2", v1[0].second);
}

}  // namespace detail
}  // namespace storage
}  // namespace core
}  // namespace jubatus
