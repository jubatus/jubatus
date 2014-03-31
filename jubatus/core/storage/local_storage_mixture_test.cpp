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
#include <vector>

#include <gtest/gtest.h>
#include "jubatus/util/data/serialization.h"
#include "jubatus/util/data/serialization/unordered_map.h"
#include "local_storage_mixture.hpp"

using std::make_pair;
using std::stringstream;
using std::sort;
using std::string;
using std::vector;

// common tests for storages are written in storage_test.cpp

namespace jubatus {
namespace core {
namespace storage {

TEST(local_storage_mixture, pack_and_unpack) {
  local_storage_mixture st;
  {
    st.set3("a", "x", val3_t(1, 11, 111));
    st.set3("a", "y", val3_t(2, 22, 222));
    st.set3("a", "z", val3_t(3, 33, 333));
    st.set3("b", "x", val3_t(12, 1212, 121212));
    st.set3("b", "z", val3_t(45, 4545, 454545));
  }

  msgpack::sbuffer buf;
  {
    msgpack::packer<msgpack::sbuffer> packer(buf);
    st.pack(packer);
  }

  local_storage_mixture st2;
  {
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, buf.data(), buf.size());
    st2.unpack(unpacked.get());
  }

  feature_val1_t a1, b1;
  st.get("a", a1);
  st.get("b", b1);
  feature_val1_t a2, b2;
  st2.get("a", a2);
  st2.get("b", b2);

  ASSERT_EQ(a1.size(), a2.size());
  for (size_t i = 0; i < a1.size(); ++i) {
    EXPECT_EQ(a1[i], a2[i]);
  }
  ASSERT_EQ(b1.size(), b2.size());
  for (size_t i = 0; i < b1.size(); ++i) {
    EXPECT_EQ(b1[i], b2[i]);
  }
}

TEST(local_storage_mixture, get_diff) {
  local_storage_mixture s;

  s.set("a", "x", 1);
  s.set("a", "y", 2);
  s.set("a", "z", 3);
  s.set("b", "x", 123);
  s.set("b", "z", 456);

  diff_t diff;
  s.get_diff(diff);

  sort(diff.diff.begin(), diff.diff.end());

  ASSERT_EQ(2u, diff.diff.size());

  EXPECT_EQ("a", diff.diff[0].first);
  feature_val3_t& a = diff.diff[0].second;
  sort(a.begin(), a.end());
  ASSERT_EQ(3u, a.size());
  EXPECT_EQ("x", a[0].first);
  EXPECT_EQ(1, a[0].second.v1);
  EXPECT_EQ("y", a[1].first);
  EXPECT_EQ(2, a[1].second.v1);
  EXPECT_EQ("z", a[2].first);
  EXPECT_EQ(3, a[2].second.v1);

  EXPECT_EQ("b", diff.diff[1].first);
  feature_val3_t& b = diff.diff[1].second;
  sort(b.begin(), b.end());
  ASSERT_EQ(2u, b.size());
  EXPECT_EQ("x", b[0].first);
  EXPECT_EQ(123, b[0].second.v1);
  EXPECT_EQ("z", b[1].first);
  EXPECT_EQ(456, b[1].second.v1);

  // update with the current diff
  s.set_average_and_clear_diff(diff);

  // update with average diff
  diff_t avg_diff;
  feature_val3_t a_diff;
  a_diff.push_back(make_pair("x", val3_t(2, 0, 0)));
  a_diff.push_back(make_pair("w", val3_t(4, 0, 0)));
  avg_diff.diff.push_back(make_pair("a", a_diff));

  feature_val3_t c_diff;
  c_diff.push_back(make_pair("x", val3_t(1, 0, 0)));
  avg_diff.diff.push_back(make_pair("c", c_diff));

  // version should be adjusted if you want mix
  avg_diff.expect_version = s.get_version();
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
    diff_t diff;
    s.get_diff(diff);
    ASSERT_EQ(0u, diff.diff.size());
  }
}

TEST(local_storage_mixture, put_diff) {
  local_storage_mixture s;
  ASSERT_EQ(0u, s.get_version().get_number());
  diff_t empty_diff;

  // this update will increment version
  s.set_average_and_clear_diff(empty_diff);
  ASSERT_EQ(1u, s.get_version().get_number());

  // this update will NOT increment version
  s.set_average_and_clear_diff(empty_diff);
  ASSERT_EQ(1u, s.get_version().get_number());

  // this update will increment version
  empty_diff.expect_version.set_number_unsafe(1LLU);
  s.set_average_and_clear_diff(empty_diff);
  ASSERT_EQ(2u, s.get_version().get_number());
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
