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

#include "waiting_mix_stage.hpp"

#include <gtest/gtest.h>
#include "stage_test_tool.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

TEST(waiting_mix_stage, general) {
  dummy_in_stage* i_stage = new dummy_in_stage();
  waiting_mix_stage* target_stage = new waiting_mix_stage();
  i_stage->set_next(target_stage);

  i_stage->go(k_dArr, k_rArr, kWeightArr);

  const window* get_diff_window1 = target_stage->get_diff();
  checkWindowIsPushedToNextStage(k_dArr, k_rArr, kWeightArr,
                                 kWindowBatchSize,
                                 kBatchInterval,
                                 get_diff_window1);

  uint32_t d_arr[] = {1, 1, 1, 1, 10};
  uint32_t r_arr[] = {0, 0, 0, 0, 2};
  double weight_arr[] = {0, 0, 0, 0, 2};

  i_stage->go(d_arr, r_arr, weight_arr);
  checkWindowIsPushedToNextStage(k_dArr, k_rArr, kWeightArr,
                                 kWindowBatchSize,
                                 kBatchInterval,
                                 get_diff_window1);

  const window* get_diff_window2 = target_stage->get_diff();
  checkWindowIsPushedToNextStage(d_arr, r_arr, weight_arr,
                                 kWindowBatchSize,
                                 kBatchInterval,
                                 get_diff_window2);
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
