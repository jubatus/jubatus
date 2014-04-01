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

#include "calculating_stage.hpp"

#include <unistd.h>
#include <iostream>
#include <vector>
#include <gtest/gtest.h>

#include "stage_test_tool.hpp"

using std::vector;

namespace jubatus {
namespace core {
namespace burst {
namespace system {

TEST(calculating_stage, general) {
  dummy_in_stage* i_stage = new dummy_in_stage();
  calculating_stage* target_stage = new calculating_stage(kScalingParam,
                                                          kGamma,
                                                          kCostcutThreshold,
                                                          kMaxReuseBatchSize);
  dummy_out_stage* o_stage = new dummy_out_stage();
  i_stage->set_next(target_stage);
  target_stage->set_next(o_stage);

  const uint32_t d_arr[] = {100, 100, 100, 100, 100};
  const uint32_t r_arr[] = {0, 0, 100, 100, 0};
  const double default_weight_arr[] = {kDefaultBatchWeight,
                                       kDefaultBatchWeight,
                                       kDefaultBatchWeight,
                                       kDefaultBatchWeight,
                                       kDefaultBatchWeight};
  const double ret_weight_arr[] = {0,
                                   0,
                                   69.31472,
                                   69.31472,
                                   0};

  i_stage->go(d_arr, r_arr, default_weight_arr);

  sleep(5);

  window* next_window = o_stage->get_window();
  vector<double> batch_weights = next_window->get_batch_weights();
  for (size_t i = 0; i < batch_weights.size(); i++) {
    std::cout << batch_weights.at(i) << std::endl;
  }
  checkWindowIsPushedToNextStage(d_arr, r_arr, ret_weight_arr,
                                 kWindowBatchSize,
                                 kBatchInterval,
                                 next_window);
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
