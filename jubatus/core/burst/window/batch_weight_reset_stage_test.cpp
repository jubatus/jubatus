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

#include "batch_weight_reset_stage.hpp"

#include <gtest/gtest.h>
#include "stage_test_tool.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

TEST(batch_weight_rest_stage, general) {
  dummy_in_stage* i_stage = new dummy_in_stage();
  batch_weight_reset_stage* target_stage = new batch_weight_reset_stage();
  dummy_out_stage* o_stage = new dummy_out_stage();
  window* next_window;
  double reset_weight_arr[] = {kDefaultBatchWeight,
                               kDefaultBatchWeight,
                               kDefaultBatchWeight,
                               kDefaultBatchWeight,
                               kDefaultBatchWeight};

  i_stage->set_next(target_stage);
  target_stage->set_next(o_stage);

  std::cout << "test: batch_weight reset" << std::endl;
  i_stage->go(k_dArr, k_rArr, kWeightArr);
  next_window = o_stage->get_window();
  checkWindowIsPushedToNextStage(k_dArr, k_rArr, reset_weight_arr,
                                 kWindowBatchSize,
                                 kBatchInterval,
                                 next_window);
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
