// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <gtest/gtest.h>

#include "clustering.hpp"
#include "clustering_config.hpp"

namespace jubatus {
namespace core {
namespace clustering {

TEST(clustering_test, config_validation) {
  std::string n("name");
  std::string m("kmeans");
  clustering_config c;

  // 1 <= k
  c.k = 0;
  ASSERT_THROW(clustering k(n, m, c), common::invalid_parameter);
  c.k = 1;
  ASSERT_NO_THROW(clustering k(n, m, c));
  c.k = 2;
  ASSERT_NO_THROW(clustering k(n, m, c));

  // 2 <= bucket_size
  c.compressed_bucket_size = 2;
  c.bicriteria_base_size = 1;
  c.bucket_size = 1;
  ASSERT_THROW(clustering k(n, m, c), common::invalid_parameter);
  // this case not will be passed;
  // (1 <= bicriteria_base_size < compressed_bucket_size < bucket_size)
  // c.bucket_size = 2;
  // ASSERT_NO_THROW(clustering k(n, m, c));
  c.bucket_size = 3;
  ASSERT_NO_THROW(clustering k(n, m, c));

  // 2 <= bucket_length
  c.bucket_length = 1;
  ASSERT_THROW(clustering k(n, m, c), common::invalid_parameter);
  c.bucket_length = 2;
  ASSERT_NO_THROW(clustering k(n, m, c));
  c.bucket_length = 3;
  ASSERT_NO_THROW(clustering k(n, m, c));

  // 1 <= bicriteria_base_size < compressed_bucket_size
  c.bucket_size = 4;
  c.compressed_bucket_size = 3;
  c.bicriteria_base_size = 0;
  ASSERT_THROW(clustering k(n, m, c), common::invalid_parameter);
  c.bicriteria_base_size = 3;
  ASSERT_THROW(clustering k(n, m, c), common::invalid_parameter);
  c.bicriteria_base_size = 4;
  ASSERT_THROW(clustering k(n, m, c), common::invalid_parameter);
  c.bicriteria_base_size = 1;
  ASSERT_NO_THROW(clustering k(n, m, c));
  c.bicriteria_base_size = 2;
  ASSERT_NO_THROW(clustering k(n, m, c));

  // compressed_bucket_size < bucket_size
  c.compressed_bucket_size = 4;
  ASSERT_THROW(clustering k(n, m, c), common::invalid_parameter);
  c.compressed_bucket_size = 5;
  ASSERT_THROW(clustering k(n, m, c), common::invalid_parameter);
  c.compressed_bucket_size = 3;
  ASSERT_NO_THROW(clustering k(n, m, c));

  // 0.0 <= forgetting_factor
  c.forgetting_factor = -1.f;
  ASSERT_THROW(clustering k(n, m, c), common::invalid_parameter);
  c.forgetting_factor = 0.f;
  ASSERT_NO_THROW(clustering k(n, m, c));
  c.forgetting_factor = 1.f;
  ASSERT_NO_THROW(clustering k(n, m, c));

  // 0.0 <= forgetting_threshold <= 1.0
  c.forgetting_threshold = -1.f;
  ASSERT_THROW(clustering k(n, m, c), common::invalid_parameter);
  c.forgetting_threshold = 0.f;
  ASSERT_NO_THROW(clustering k(n, m, c));
  c.forgetting_threshold = 0.5f;
  ASSERT_NO_THROW(clustering k(n, m, c));
  c.forgetting_threshold = 1.f;
  ASSERT_NO_THROW(clustering k(n, m, c));
  c.forgetting_threshold = 2.f;
  ASSERT_THROW(clustering k(n, m, c), common::invalid_parameter);
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
