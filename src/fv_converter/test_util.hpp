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

#pragma once

#include <utility>
#include <vector>
#include <gtest/gtest.h>

namespace jubatus {

// TODO(beam2d): Move this function to common or consider using ASSERT_EQ
// directly.  If we continue using it, we must also rename it to follow our
// convention.
template<typename S, typename T>
void PairVectorEquals(const std::vector<std::pair<S, T> >& expected,
                      const std::vector<std::pair<S, T> >& actual) {
  ASSERT_EQ(expected.size(), actual.size()) << "unequal length";
  for (size_t i = 0; i < expected.size(); ++i) {
    const std::pair<S, T>& e = expected[i];
    const std::pair<S, T>& a = actual[i];
    ASSERT_EQ(e.first, a.first) << "(" << e.first << ", " << e.second << ") != "
        << "(" << a.first << ", " << a.second << ")";
    ASSERT_EQ(e.second, a.second) << "(" << e.first << ", " << e.second
        << ") != " << "(" << a.first << ", " << a.second << ")";
  }
}

}  // namespace jubatus

