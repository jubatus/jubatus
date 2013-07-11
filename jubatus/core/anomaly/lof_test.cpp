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
#include <pficommon/data/unordered_map.h>
#include <pficommon/text/json.h>
#include "../common/jsonconfig.hpp"
#include "../recommender/recommender.hpp"
#include "lof_storage.hpp"

using jubatus::core::storage::lof_storage;
using pfi::data::unordered_map;
using pfi::lang::shared_ptr;
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

REGISTER_TYPED_TEST_CASE_P(lof_test, update_row);

typedef testing::Types<recommender::inverted_index, recommender::lsh,
  recommender::minhash, recommender::euclid_lsh> recommender_types;

INSTANTIATE_TYPED_TEST_CASE_P(lt,
  lof_test, recommender_types);

}  // namespace anomaly
}  // namespace core
}  // namespace jubatus
