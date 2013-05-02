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
#include <pficommon/lang/scoped_ptr.h>

#include "recommender_mock.hpp"
#include "recommender_mock_util.hpp"

using std::pair;
using std::string;
using std::vector;
using pfi::lang::scoped_ptr;

namespace jubatus {
namespace core {
namespace recommender {

TEST(recommender_mock, similar_row_by_id) {
  recommender_mock r;
  r.update_row("r1", make_sfv("f1:1 f2:1"));

  const vector<pair<string, float> > expect = make_ids("r2:2 r3:1");
  r.set_similar_relation("r1", expect);

  vector<pair<string, float> > ids;
  r.similar_row("r1", ids, 2);

  EXPECT_EQ(expect, ids);
}

TEST(recommender_mock, neighbor_row_by_id) {
  recommender_mock r;
  r.update_row("r1", make_sfv("f1:1 f2:1"));

  const vector<pair<string, float> > expect = make_ids("r2:1 r3:2");
  r.set_neighbor_relation("r1", expect);

  vector<pair<string, float> > ids;
  r.neighbor_row("r1", ids, 2);

  EXPECT_EQ(expect, ids);
}

TEST(recommender_mock, similar_row_by_query) {
  recommender_mock r;

  const sfv_t query = make_sfv("f1:1 f2:1");
  const vector<pair<string, float> > expect = make_ids("r2:2 r3:1");
  r.set_similar_relation(query, expect);

  vector<pair<string, float> > ids;
  r.similar_row(query, ids, 2);

  EXPECT_EQ(expect, ids);
}

TEST(recommender_mock, neighbor_row_by_query) {
  recommender_mock r;

  const sfv_t query = make_sfv("f1:1 f2:1");
  const vector<pair<string, float> > expect = make_ids("r2:1 r3:2");
  r.set_neighbor_relation(query, expect);

  vector<pair<string, float> > ids;
  r.neighbor_row(query, ids, 2);

  EXPECT_EQ(expect, ids);
}

TEST(recommender_mock, name) {
  scoped_ptr<recommender_base> r(new recommender_mock);
  EXPECT_EQ("recommender_mock", r->type());
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
