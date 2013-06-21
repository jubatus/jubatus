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

#include <cmath>
#include <utility>
#include <gtest/gtest.h>
#include "mixable_stat.hpp"

using std::pair;

namespace jubatus {

TEST(mixable_stat_test, mixed_entropy) {
  core::stat::mixable_stat p(1024);
  p.push("test", 1.0);
  p.push("test", 2.0);
  p.push("test", 3.0);

  double e_d = 3 * log(3);
  double e_e = - e_d / 3 + log(3);

  std::pair<double, size_t> d = p.get_diff();
  ASSERT_DOUBLE_EQ(e_d, d.first);
  ASSERT_EQ(3u, d.second);

  p.put_diff(d);

  ASSERT_DOUBLE_EQ(e_e, p.entropy());
  ASSERT_DOUBLE_EQ(p.entropy(), p.entropy());
}
}  // namespace jubatus
