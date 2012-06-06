// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <sstream>
#include <gtest/gtest.h>
#include <iostream> 

#include "../common/type.hpp"
#include "similarity_base.hpp"

#include "similarity_factory.hpp"
#include "anchor_finder_factory.hpp"
#include "anchor_builder_factory.hpp"
#include "recommender_builder.hpp"
#include "recommender.hpp"
#include "recommender_type.hpp"

using namespace std;

// generate sfv_diff_t.
void init_sfv_diff(size_t i, jubatus::sfv_diff_t &sfv_diff) {

  for (size_t j = 0; j < 10; ++j) {
    stringstream ss;
    if (i % 2 == 0 && j % 2 == 0) {
      ss << j;
      sfv_diff.push_back(make_pair(ss.str(), 1.0));
    } else if (i % 2 == 1 && j % 2 == 1) {
      ss << j;
      sfv_diff.push_back(make_pair(ss.str(), 1.0));
    }
  }
}

// generate sfv_diff_t.
void init_sfv(size_t i, jubatus::sfv_t &sfv) {

  for (size_t j = 0; j < 10; ++j) {
    stringstream ss;
    if (i % 2 == 0 && j % 2 == 0) {
      ss << j;
      sfv.push_back(make_pair(ss.str(), 1.0));
    } else if (i % 2 == 1 && j % 2 == 1) {
      ss << j;
      sfv.push_back(make_pair(ss.str(), 1.0));
    }
  }
}

void similar_row_test(const char* similarity_name,
                      const char* anchor_finder_name,
                      const char* anchor_builder_name){
  jubatus::recommender::recommender_builder builder(similarity_name, anchor_finder_name, anchor_builder_name);

  for (size_t i = 0; i < 100; ++i) {
    jubatus::sfv_diff_t sfv_diff;
    init_sfv_diff(i, sfv_diff);

    stringstream ss;
    ss << i;
    builder.update_row(ss.str(), sfv_diff);
  }

  jubatus::recommender::recommender recommender;
  builder.build(recommender, 15, 3, recommender);

  for (size_t i = 0; i < 100; ++i) {
    jubatus::sfv_t sfv;
    init_sfv(i, sfv);

    std::vector<std::pair<std::string, float> > ids;
    recommender.similar_row(sfv, ids, 10);
    ASSERT_TRUE(ids.size() == 10);

    for (size_t j = 0; j < ids.size(); ++j) {
      istringstream is(ids[j].first);
      size_t result_id;
      is >> result_id;
      ASSERT_TRUE(i % 2 == result_id % 2);
    }
  }
}

// TODO(hillbig) use parameterized value test

TEST(recommender_builder, artificial_data_euclid_naive_random){
  similar_row_test("euclid", "naive",  "random");
}

TEST(recommender_builder, artificial_data_euclid_knndecent_random){
  similar_row_test("euclid", "knndecent",  "random");
}

TEST(recommender_builder, artificial_data_cos_naive_random){
  similar_row_test("cos", "naive",  "random");
}

TEST(recommender_builder, artificial_data_cos_knndecent_random){
  similar_row_test("cos", "knndecent",  "random");
}
