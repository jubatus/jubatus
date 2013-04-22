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

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "sparse_matrix_storage.hpp"
#include "norm.hpp"

using std::make_pair;
using std::pair;
using std::string;
using std::stringstream;
using std::sort;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {

TEST(sparse_matrix_storage, empty) {
  sparse_matrix_storage s;
  EXPECT_EQ(0.0, s.get("row", "column"));

  vector<pair<string, float> > row;
  s.get_row("row", row);
  EXPECT_TRUE(row.empty());

  vector<string> ids;
  s.get_all_row_ids(ids);
  EXPECT_TRUE(ids.empty());
}

TEST(sparse_matrix_storage, set_row) {
  sparse_matrix_storage s;
  vector<pair<string, float> > r1, r2;
  r1.push_back(make_pair("c1", 1.0));
  r1.push_back(make_pair("c2", 2.0));

  s.set_row("r1", r1);
  r2.push_back(make_pair("c2", 4.0));
  r2.push_back(make_pair("c3", 5.0));
  s.set_row("r2", r2);

  vector<pair<string, float> > p;
  s.get_row("r1", p);
  ASSERT_EQ(r1, p);

  EXPECT_EQ(2.0, s.get("r1", "c2"));
  EXPECT_EQ(0.0, s.get("unknown", "c2"));
  EXPECT_EQ(0.0, s.get("r1", "unknown"));
  EXPECT_EQ(0.0, s.get("r1", "r1"));
  EXPECT_EQ(0.0, s.get("r1", "c3"));

  vector<string> ids;
  s.get_all_row_ids(ids);
  ASSERT_EQ(2u, ids.size());
  sort(ids.begin(), ids.end());
  EXPECT_EQ("r1", ids[0]);
  EXPECT_EQ("r2", ids[1]);
}

TEST(sparse_matrix_storage, set) {
  sparse_matrix_storage s;
  s.set("r1", "c1", 5.0);
  // overwrite
  s.set("r1", "c1", 1.0);
  s.set("r2", "c2", 4.0);
  s.set("r1", "c2", 2.0);
  s.set("r2", "c3", 6.0);

  EXPECT_EQ(1.0, s.get("r1", "c1"));
  EXPECT_EQ(4.0, s.get("r2", "c2"));
  EXPECT_EQ(2.0, s.get("r1", "c2"));
  EXPECT_EQ(6.0, s.get("r2", "c3"));

  EXPECT_EQ(0.0, s.get("r0", "c1"));
  EXPECT_EQ(0.0, s.get("r0", "c0"));
  EXPECT_EQ(0.0, s.get("r1", "c0"));

  vector<pair<string, float> > p;
  s.get_row("r1", p);
  ASSERT_EQ(2u, p.size());
  sort(p.begin(), p.end());
  EXPECT_EQ("c1", p[0].first);
  EXPECT_EQ(1.0, p[0].second);
  EXPECT_EQ("c2", p[1].first);
  EXPECT_EQ(2.0, p[1].second);

  vector<string> ids;
  s.get_all_row_ids(ids);
  ASSERT_EQ(2u, ids.size());
  sort(ids.begin(), ids.end());
  EXPECT_EQ("r1", ids[0]);
  EXPECT_EQ("r2", ids[1]);

  s.clear();
  EXPECT_EQ(0.0, s.get("r1", "c1"));
}

TEST(sparse_matrix_storage, calc_l2norm) {
  sparse_matrix_storage s;
  EXPECT_FLOAT_EQ(0.0, s.calc_l2norm("unknown"));
  s.set("r1", "c1", 1.0);
  s.set("r1", "c2", 2.0);
  s.set("r1", "c3", 3.0);
  EXPECT_FLOAT_EQ(std::sqrt(14.0), s.calc_l2norm("r1"));
}

TEST(sparse_matrix_storage, save_load) {
  sparse_matrix_storage s;
  s.set("r1", "c1", 1.0);
  stringstream ss;
  s.save(ss);

  sparse_matrix_storage s2;
  s2.load(ss);
  EXPECT_FLOAT_EQ(1.0, s2.get("r1", "c1"));
}

TEST(sparse_matrix_storage, remove) {
  sparse_matrix_storage s;
  s.set("r1", "c1", 1.0);
  EXPECT_EQ(1.0, s.get("r1", "c1"));
  s.remove("r1", "c1");
  EXPECT_EQ(0.0, s.get("r1", "c1"));

  vector<string> ids;
  s.get_all_row_ids(ids);
  // EXPECT_TRUE(ids.empty()); // it does not work
}

TEST(sparse_matrix_storage, remove_row) {
  sparse_matrix_storage s;
  s.set("r1", "c1", 1.0);
  EXPECT_EQ(1.0, s.get("r1", "c1"));
  s.remove_row("r1");
  EXPECT_EQ(0.0, s.get("r1", "c1"));

  vector<string> ids;
  s.get_all_row_ids(ids);
  EXPECT_TRUE(ids.empty());
}

TEST(sparse_matrix_storage, remove_invalid_rows) {
  sparse_matrix_storage s;
  s.set("r1", "c1", 1.0);
  s.remove("unknow", "c1");
  s.remove("r1", "unknwon");
  s.remove_row("unknown");
  EXPECT_EQ(1.0, s.get("r1", "c1"));
}

TEST(sparse_matrix_storage, clear) {
  sparse_matrix_storage s;
  s.set("r1", "c1", 1.0);
  s.clear();
  EXPECT_EQ(0.0, s.get("r1", "c1"));
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
