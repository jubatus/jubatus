// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <gtest/gtest.h>
#include <pficommon/data/string/utility.h>
#include <pficommon/lang/cast.h>
#include <pficommon/lang/scoped_ptr.h>

#include "bit_vector_nearest_neighbor_base.hpp"

using std::vector;
using std::map;
using std::pair;
using std::string;
using std::make_pair;
using jubatus::table::bit_vector;
using jubatus::table::column_table;
using pfi::data::string::split;

namespace jubatus {
namespace nearest_neighbor {

namespace {
bit_vector make_bv(const string& in_str) {
  bit_vector bv(in_str.size());
  for (size_t i = 0; i < in_str.size(); ++i) {
    if (in_str[i] == '1') {
      bv.set_bit(i);
    }
  }
  return bv;
}

sfv_t make_sfv(const string& in_str) {
  sfv_t sfv;
  vector<string> cells = split(in_str, ' ');
  for (size_t i = 0; i < cells.size(); ++i) {
    vector<string> pair = split(cells[i], ':');
    sfv.push_back(make_pair(pair[0], pfi::lang::lexical_cast<float>(pair[1])));
  }
  return sfv;
}
}

class bit_vector_nearest_neighbor_mock
  : public bit_vector_nearest_neighbor_base {
 public:
  bit_vector_nearest_neighbor_mock(uint64_t bitnum,
                                   table::column_table* table)
      : bit_vector_nearest_neighbor_base(bitnum, table, "test"),
        hash_value_(bitnum) {}

  void set_next_bv(const bit_vector& bv) {
    hash_value_ = bv;
  }

  void add_bv(const string& id, const bit_vector& bv) {
    set_next_bv(bv);
    set_row(id, sfv_t());
  }

  virtual string type() const { return "bv_mock"; }

 protected:
  virtual bit_vector hash(const sfv_t&) const {
    return hash_value_;
  }

 private:
  bit_vector hash_value_;
};

class bit_vector_nearest_neighbor_base_test : public testing::Test {
 protected:
  void set_bitnum(uint64_t bitnum) {
    ct_.reset(new column_table);
    mock_.reset(new bit_vector_nearest_neighbor_mock(bitnum, ct_.get()));
  }

  pfi::lang::scoped_ptr<column_table> ct_;
  pfi::lang::scoped_ptr<bit_vector_nearest_neighbor_mock> mock_;
};

TEST_F(bit_vector_nearest_neighbor_base_test, get_all_row_ids) {
  set_bitnum(1);
  mock_->add_bv("A", make_bv("0"));
  mock_->add_bv("B", make_bv("1"));

  vector<string> actual, expect;
  mock_->get_all_row_ids(actual);
  expect.push_back("A");
  expect.push_back("B");

  EXPECT_EQ(expect, actual);
}

TEST_F(bit_vector_nearest_neighbor_base_test, neighbor_row_from_data) {
  set_bitnum(4);
  mock_->add_bv("A", make_bv("0000"));
  mock_->add_bv("B", make_bv("0001"));
  mock_->add_bv("C", make_bv("0111"));
  mock_->add_bv("D", make_bv("1100"));

  vector<pair<string, float> > actual_neighbors, expect_neighbors;
  mock_->set_next_bv(make_bv("1111"));
  mock_->neighbor_row(sfv_t(), actual_neighbors, 3);

  expect_neighbors.push_back(make_pair("C", .25f));
  expect_neighbors.push_back(make_pair("D", .5f));
  expect_neighbors.push_back(make_pair("B", .75f));

  EXPECT_EQ(expect_neighbors, actual_neighbors);

  vector<pair<string, float> > actual_similars, expect_similars;

  mock_->similar_row(sfv_t(), actual_similars, 3);
  expect_similars.push_back(make_pair("C", .75f));
  expect_similars.push_back(make_pair("D", .5f));
  expect_similars.push_back(make_pair("B", .25f));

  EXPECT_EQ(expect_similars, actual_similars);
}

TEST_F(bit_vector_nearest_neighbor_base_test, neighbor_row_from_id) {
  set_bitnum(4);
  mock_->add_bv("A", make_bv("0000"));
  mock_->add_bv("B", make_bv("0001"));
  mock_->add_bv("C", make_bv("1111"));
  mock_->add_bv("D", make_bv("1100"));

  vector<pair<string, float> > actual_neighbors, expect_neighbors;
  mock_->neighbor_row("C", actual_neighbors, 3);
  expect_neighbors.push_back(make_pair("C", .0f));
  expect_neighbors.push_back(make_pair("D", .5f));
  expect_neighbors.push_back(make_pair("B", .75f));

  EXPECT_EQ(expect_neighbors, actual_neighbors);

  vector<pair<string, float> > actual_similars, expect_similars;
  mock_->similar_row("C", actual_similars, 3);
  expect_similars.push_back(make_pair("C", 1.f));
  expect_similars.push_back(make_pair("D", .5f));
  expect_similars.push_back(make_pair("B", .25f));

  EXPECT_EQ(expect_similars, actual_similars);
}

}  // namespace nearest_neighbor
}  // namespace jubatus
