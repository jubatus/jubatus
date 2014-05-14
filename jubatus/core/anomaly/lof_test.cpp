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

#include "lof.hpp"

#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "jubatus/util/data/unordered_map.h"
#include "jubatus/util/text/json.h"
#include "../common/jsonconfig.hpp"
#include "../recommender/recommender.hpp"
#include "lof_storage.hpp"

using jubatus::util::data::unordered_map;
using jubatus::util::lang::shared_ptr;
using std::pair;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace anomaly {

template<typename T>
class lof_test : public testing::Test {
};

TYPED_TEST_CASE_P(lof_test);

TYPED_TEST_P(lof_test, update_row) {
  lof l(lof_storage::config(), shared_ptr<TypeParam>(new TypeParam));
  common::sfv_t v, q;
  const string id = "test";
  l.update_row(id, v);
  l.calc_anomaly_score(id);
}

TYPED_TEST_P(lof_test, config_validation) {
  shared_ptr<TypeParam> nn_engine(new TypeParam);
  shared_ptr<lof> l;
  lof_storage::config c;

  c.reverse_nearest_neighbor_num = 10;

  // nearest_neighbor_num <= 2
  c.nearest_neighbor_num = 1;
  ASSERT_THROW(l.reset(new lof(c, nn_engine)), common::invalid_parameter);

  c.nearest_neighbor_num = 2;
  ASSERT_NO_THROW(l.reset(new lof(c, nn_engine)));

  c.nearest_neighbor_num = 3;
  ASSERT_NO_THROW(l.reset(new lof(c, nn_engine)));

  // nearest_neighbor_num <= reverse_nearest_neighbor_num
  c.reverse_nearest_neighbor_num = 2;
  ASSERT_THROW(l.reset(new lof(c, nn_engine)), common::invalid_parameter);

  c.reverse_nearest_neighbor_num = 3;
  ASSERT_NO_THROW(l.reset(new lof(c, nn_engine)));
}

REGISTER_TYPED_TEST_CASE_P(lof_test, update_row, config_validation);

typedef testing::Types<recommender::inverted_index, recommender::lsh,
  recommender::minhash, recommender::euclid_lsh> recommender_types;

INSTANTIATE_TYPED_TEST_CASE_P(lt,
  lof_test, recommender_types);

}  // namespace anomaly
}  // namespace core
}  // namespace jubatus
