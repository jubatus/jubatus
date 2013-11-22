// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <vector>
#include <gtest/gtest.h>
#include "jubatus/util/lang/scoped_ptr.h"
#include "../common/type.hpp"
#include "clustering.hpp"
#include "clustering_config.hpp"
#include "types.hpp"
#include "testutil.hpp"

using std::vector;
using jubatus::core::common::sfv_t;

namespace jubatus {
namespace core {
namespace clustering {

class model_test : public ::testing::Test {
 protected:
  static const size_t k_ = 2;

  model_test() {
    config_.k = 3;
    config_.compressor_method = "compressive_kmeans";
    config_.bucket_size = 10000;
    config_.compressed_bucket_size = 100;
    config_.forgetting_threshold = 0.05;
    config_.forgetting_factor = 2;
    config_.bucket_length = 4;

    model_.reset(new clustering("test", "kmeans", config_));
  }

  jubatus::util::lang::scoped_ptr<clustering> model_;
  clustering_config config_;
};

TEST_F(model_test, initial_centers) {
  vector<common::sfv_t> centers = model_->get_k_center();
  ASSERT_EQ(centers.size(), 0ul);
}

TEST_F(model_test, push_small) {
  static const size_t N = 100;
  static const size_t D = 2;
  model_->push(get_points(N, D));
  vector<weighted_point> coreset = model_->get_coreset();
  ASSERT_EQ(coreset.size(), N);
  ASSERT_EQ(coreset.front().data.size(), D);

  vector<common::sfv_t> centers = model_->get_k_center();
  ASSERT_EQ(centers.size(), 0ul);
}

TEST_F(model_test, compression_and_clusteringing) {
  static const size_t N = config_.bucket_size;
  static const size_t D = 2;
  std::cout << "total adding points : " << N << std::endl;
  model_->push(get_points(N, D));
  vector<weighted_point> coreset = model_->get_coreset();
  ASSERT_EQ(
      coreset.size(), static_cast<size_t>(config_.compressed_bucket_size));

  vector<common::sfv_t> centers = model_->get_k_center();
  ASSERT_EQ(centers.size(), static_cast<size_t>(config_.k));

  vector<wplist> core_members = model_->get_core_members();
  ASSERT_GT(core_members.size(), 0ul);

  wplist nearest_members = model_->get_nearest_members(get_point(D).data);
  ASSERT_GT(nearest_members.size(), 0ul);

  common::sfv_t nearest_center = model_->get_nearest_center(get_point(D).data);
}


TEST_F(model_test, bucket_management_and_forgetting) {
  static const size_t N = config_.bucket_size * 4;
  static const size_t D = 2;
  model_->push(get_points(N, D));
  vector<weighted_point> coreset = model_->get_coreset();
  ASSERT_EQ(
      coreset.size(), static_cast<size_t>(config_.compressed_bucket_size));

  model_->push(get_points(N, D));
  coreset = model_->get_coreset();
  ASSERT_EQ(
      coreset.size(), static_cast<size_t>(config_.compressed_bucket_size));
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
