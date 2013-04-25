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

#include <algorithm>
#include <fstream>
#include <string>

#include <gtest/gtest.h>
#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include "local_storage_mixture.hpp"

using std::make_pair;
using std::stringstream;
using std::sort;

// common tests for storages are written in storage_test.cpp

namespace jubatus {
namespace core {
namespace storage {

TEST(local_storage_mixture, save_load) {
  local_storage_mixture st;
  {
    st.set3("a", "x", val3_t(1, 11, 111));
    st.set3("a", "y", val3_t(2, 22, 222));
    st.set3("a", "z", val3_t(3, 33, 333));
    st.set3("b", "x", val3_t(12, 1212, 121212));
    st.set3("b", "z", val3_t(45, 4545, 454545));
  }
  stringstream ss;
  st.save(ss);
}

TEST(local_storage_mixture, get_diff) {
  local_storage_mixture s;

  s.set("a", "x", 1);
  s.set("a", "y", 2);
  s.set("a", "z", 3);
  s.set("b", "x", 123);
  s.set("b", "z", 456);

  features3_t diff;
  s.get_diff(diff);

  sort(diff.begin(), diff.end());

  ASSERT_EQ(2u, diff.size());

  EXPECT_EQ("a", diff[0].first);
  feature_val3_t& a = diff[0].second;
  sort(a.begin(), a.end());
  ASSERT_EQ(3u, a.size());
  EXPECT_EQ("x", a[0].first);
  EXPECT_EQ(1, a[0].second.v1);
  EXPECT_EQ("y", a[1].first);
  EXPECT_EQ(2, a[1].second.v1);
  EXPECT_EQ("z", a[2].first);
  EXPECT_EQ(3, a[2].second.v1);

  EXPECT_EQ("b", diff[1].first);
  feature_val3_t& b = diff[1].second;
  sort(b.begin(), b.end());
  ASSERT_EQ(2u, b.size());
  EXPECT_EQ("x", b[0].first);
  EXPECT_EQ(123, b[0].second.v1);
  EXPECT_EQ("z", b[1].first);
  EXPECT_EQ(456, b[1].second.v1);

  // update with the current diff
  s.set_average_and_clear_diff(diff);

  // update with average diff
  features3_t avg_diff;
  feature_val3_t a_diff;
  a_diff.push_back(make_pair("x", val3_t(2, 0, 0)));
  a_diff.push_back(make_pair("w", val3_t(4, 0, 0)));
  avg_diff.push_back(make_pair("a", a_diff));

  feature_val3_t c_diff;
  c_diff.push_back(make_pair("x", val3_t(1, 0, 0)));
  avg_diff.push_back(make_pair("c", c_diff));

  s.set_average_and_clear_diff(avg_diff);

  // now the feature vector is expected as below
  // a: x = 1 + 2 = 3
  //    y = 2
  //    z = 3
  //    w = 4
  // b: x = 123
  //    z = 456
  // c: x = 1
  {
    feature_val1_t v;
    s.get("a", v);
    sort(v.begin(), v.end());

    ASSERT_EQ(4u, v.size());
    EXPECT_EQ("w", v[0].first);
    EXPECT_EQ(4, v[0].second);
    EXPECT_EQ("x", v[1].first);
    EXPECT_EQ(3, v[1].second);
    EXPECT_EQ("y", v[2].first);
    EXPECT_EQ(2, v[2].second);
    EXPECT_EQ("z", v[3].first);
    EXPECT_EQ(3, v[3].second);
  }
  {
    feature_val1_t v;
    s.get("b", v);
    sort(v.begin(), v.end());

    ASSERT_EQ(2u, v.size());
    EXPECT_EQ("x", v[0].first);
    EXPECT_EQ(123, v[0].second);
    EXPECT_EQ("z", v[1].first);
    EXPECT_EQ(456, v[1].second);
  }
  {
    feature_val1_t v;
    s.get("c", v);
    sort(v.begin(), v.end());

    ASSERT_EQ(1u, v.size());
    EXPECT_EQ("x", v[0].first);
    EXPECT_EQ(1, v[0].second);
  }
  {
    features3_t diff;
    s.get_diff(diff);
    ASSERT_EQ(0u, diff.size());
  }
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
