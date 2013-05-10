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
#include <utility>
#include <vector>
#include <gtest/gtest.h>

#include <pficommon/lang/scoped_ptr.h>

#include "nearest_neighbor_base.hpp"

namespace jubatus {
namespace nearest_neighbor {

using std::string;
using std::vector;
using std::pair;
using std::make_pair;

class nearest_neighbor_mock : public nearest_neighbor_base {
 public:
  explicit nearest_neighbor_mock(table::column_table* table)
      : nearest_neighbor_base(table, "test") {}

  void add_next_answer(const string& id, float dist) {
    answer_.push_back(make_pair(id, dist));
  }

  virtual string type() const { return "nearest_neighbor_mock"; }

  virtual void set_row(const string&, const sfv_t&) {}

  virtual void neighbor_row(const sfv_t&,
                            vector<pair<string, float> >& ids,
                            uint64_t ret_num) const {
    ids = answer_;
    if (ids.size() > ret_num) {
      ids.resize(ret_num);
    }
  }

  virtual void neighbor_row(const string&,
                            vector<pair<string, float> >& ids,
                            uint64_t ret_num) const {
    ids = answer_;
    if (ids.size() > ret_num) {
      ids.resize(ret_num);
    }
  }

 private:
  vector<pair<string, float> > answer_;
};

class nearest_neighbor_base_test : public testing::Test {
 protected:
  virtual void SetUp() {
    mock_.reset(new nearest_neighbor_mock(&ct_));
  }

  table::column_table ct_;
  pfi::lang::scoped_ptr<nearest_neighbor_mock> mock_;
};

TEST_F(nearest_neighbor_base_test, get_table) {
  EXPECT_EQ(&ct_, mock_->get_table());
}

TEST_F(nearest_neighbor_base_test, get_const_table) {
  const nearest_neighbor_base* cmock = mock_.get();
  EXPECT_EQ(&ct_, cmock->get_const_table());
}

TEST_F(nearest_neighbor_base_test, order_of_calc_similarity) {
  EXPECT_GT(mock_->calc_similarity(0), mock_->calc_similarity(0.5));
  EXPECT_GT(mock_->calc_similarity(0.5), mock_->calc_similarity(1));
}

TEST_F(nearest_neighbor_base_test, similar_row) {
  mock_->add_next_answer("a", 0);
  mock_->add_next_answer("a", 0.25);
  mock_->add_next_answer("a", 0.5);
  mock_->add_next_answer("a", 0.75);
  mock_->add_next_answer("a", 1);

  vector<pair<string, float> > neighbors, similars;
  mock_->neighbor_row("", neighbors, 5);
  mock_->similar_row("", similars, 5);

  EXPECT_EQ(neighbors.size(), similars.size());
  for (size_t i = 0; i < neighbors.size(); ++i) {
    EXPECT_EQ(neighbors[i].first, similars[i].first);
    EXPECT_EQ(mock_->calc_similarity(neighbors[i].second), similars[i].second);
  }
}

}  // namespace nearest_neighbor
}  // namespace jubatus
