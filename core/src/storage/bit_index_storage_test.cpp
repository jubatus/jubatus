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

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "bit_index_storage.hpp"

using std::pair;
using std::stringstream;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {

bit_vector make_vector(const string& b) {
  bit_vector v;
  v.resize_and_clear(b.size());
  for (size_t i = 0; i < b.size(); ++i) {
    if (b[i] == '1') {
      v.set_bit(i);
    }
  }
  return v;
}

TEST(bit_index_storage, trivial) {
  bit_index_storage s;
  EXPECT_EQ("bit_index_storage", s.name());

  s.set_row("r1", make_vector("0101"));
  s.set_row("r2", make_vector("1010"));
  s.set_row("r3", make_vector("1110"));
  s.set_row("r4", make_vector("1100"));

  vector<pair<string, float> > ids;
  s.similar_row(make_vector("1100"), ids, 2);

  ASSERT_EQ(2u, ids.size());
  EXPECT_EQ("r4", ids[0].first);
  EXPECT_FLOAT_EQ(1.0, ids[0].second);
  EXPECT_EQ("r3", ids[1].first);
  EXPECT_FLOAT_EQ(0.75, ids[1].second);

  stringstream ss;
  s.save(ss);
  bit_index_storage t;
  t.load(ss);
  ids.clear();
  t.similar_row(make_vector("1100"), ids, 2);
  ASSERT_EQ(2u, ids.size());
  EXPECT_EQ("r4", ids[0].first);

  s.remove_row("r4");
  ids.clear();
  s.similar_row(make_vector("1100"), ids, 2);
  EXPECT_NE("r4", ids[0].first);

  s.clear();
  ids.clear();
  s.similar_row(make_vector("1100"), ids, 2);
  EXPECT_TRUE(ids.empty());
}

TEST(bit_index_storage, diff) {
  bit_index_storage s1, s2;
  s1.set_row("r1", make_vector("0101"));
  s1.set_row("r2", make_vector("1010"));
  string d1;
  s1.get_diff(d1);

  s2.set_mixed_and_clear_diff(d1);
  bit_vector v;
  s2.get_row("r1", v);
  EXPECT_TRUE(make_vector("0101") == v);

  v.resize_and_clear(4);
  s2.get_row("r2", v);
  EXPECT_TRUE(make_vector("1010") == v);
}

TEST(bit_index_storage, mix) {
  bit_index_storage s1, s2, s3;
  s1.set_row("r1", make_vector("0101"));
  s1.set_row("r2", make_vector("1010"));
  string d1;
  s1.get_diff(d1);

  s2.set_row("r1", make_vector("1110"));
  s2.set_row("r3", make_vector("1100"));
  string d2;
  s2.get_diff(d2);

  s1.mix(d1, d2);

  // d2 is
  // r1: 0101 (s1)
  // r2: 1010 (s1)
  // r3: 1100 (s2)

  s3.set_row("r1", make_vector("1111"));
  s3.set_row("r2", make_vector("1111"));
  s3.set_row("r3", make_vector("1111"));
  s3.set_row("r4", make_vector("1111"));
  s3.set_mixed_and_clear_diff(d2);

  // r1, r2 and r3 are overwritten by d2
  // r4 is no longer retained

  bit_vector v;
  s3.get_row("r1", v);
  EXPECT_TRUE(v == make_vector("0101"));
  s3.get_row("r2", v);
  EXPECT_TRUE(v == make_vector("1010"));
  s3.get_row("r3", v);
  EXPECT_TRUE(v == make_vector("1100"));
  s3.get_row("r4", v);
  EXPECT_TRUE(v == bit_vector());
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
