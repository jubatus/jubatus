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
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "../storage/norm.hpp"
#include "recommender_base.hpp"
#include "../storage/recommender_storage.hpp"
#include "../storage/norm_none.hpp"

using std::make_pair;
using std::pair;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace recommender {

class recommender_impl : public recommender_base {
 public:
  recommender_impl()
      : recommender_base() {
    // make mock
    orig_.set("r1", "a1", 1.0);
    orig_.set("r1", "a2", 1.0);

    orig_.set("r2", "b1", 1.0);
    orig_.set("r2", "b2", 1.0);

    orig_.set("r3", "a1", 1.0);
    orig_.set("r3", "b1", 1.0);
  }

  void similar_row(
      const sfv_t& query,
      vector<pair<string, float> >& ids,
      size_t ret_num) const {
    ids.clear();
    ids.push_back(make_pair("r1", 2.0));
    ids.push_back(make_pair("r3", 1.0));
  }

  void neighbor_row(
      const sfv_t& query,
      vector<pair<string, float> >& ids,
      size_t ret_num) const {
    ids.clear();
    ids.push_back(make_pair("r1", 1.0));
    ids.push_back(make_pair("r3", 2.0));
  }

  void clear() {
  }

  void clear_row(const string& id) {
  }

  void update_row(const string& id, const sfv_diff_t& diff) {
  }

  void get_all_row_ids(vector<string>& ids) const {
    ids.clear();
    ids.push_back("r1");
    ids.push_back("r2");
    ids.push_back("r3");
  }

  string type() const {
    return string("recommender_impl");
  }

  bool save_impl(std::ostream&) {
    return true;
  }

  bool load_impl(std::istream&) {
    return true;
  }

  storage::recommender_storage_base* get_storage() {
    return NULL;
  }

  const storage::recommender_storage_base* get_const_storage() const {
    return NULL;
  }
};

TEST(recommender_base, complete_row) {
  recommender_impl r;
  sfv_t q;
  sfv_t ret;
  r.complete_row(q, ret);
  ASSERT_EQ(3u, ret.size());
  EXPECT_EQ("a1", ret[0].first);
  EXPECT_EQ("a2", ret[1].first);
  EXPECT_EQ("b1", ret[2].first);
}

TEST(recommender_base, get_all_row_ids) {
  vector<string> ids;
  recommender_impl r;
  r.get_all_row_ids(ids);
  ASSERT_EQ(3u, ids.size());
  sort(ids.begin(), ids.end());
  EXPECT_EQ("r1", ids[0]);
  EXPECT_EQ("r2", ids[1]);
  EXPECT_EQ("r3", ids[2]);
}

TEST(recommender_base, decode_row) {
  recommender_impl r;
  sfv_t v;
  r.decode_row("r1", v);
  ASSERT_EQ(2u, v.size());
  EXPECT_EQ("a1", v[0].first);
  EXPECT_EQ(1.0, v[0].second);
  EXPECT_EQ("a2", v[1].first);
  EXPECT_EQ(1.0, v[1].second);

  r.decode_row("r", v);
  ASSERT_TRUE(v.empty());
}

TEST(recommender_base, calc_l2norm) {
  recommender_impl r;
  sfv_t q;
  q.push_back(make_pair("a1", 1.0));
  q.push_back(make_pair("a2", 2.0));
  q.push_back(make_pair("a3", 3.0));

  EXPECT_FLOAT_EQ(sqrt(1.0 + 4.0 + 9.0), r.calc_l2norm(q));
}

TEST(recommender_base, calc_similality) {
  recommender_impl r;
  sfv_t q1;
  sfv_t q2;

  q1.push_back(make_pair("c1", 1.0));
  q1.push_back(make_pair("c2", 2.0));
  q1.push_back(make_pair("c3", 3.0));

  q2.push_back(make_pair("c4", 2.0));
  q2.push_back(make_pair("c3", 3.0));
  q2.push_back(make_pair("c2", 3.0));

  EXPECT_FLOAT_EQ(
      (2.0 * 3.0 + 3.0 * 3.0) / r.calc_l2norm(q1) / r.calc_l2norm(q2),
      r.calc_similality(q1, q2));
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
