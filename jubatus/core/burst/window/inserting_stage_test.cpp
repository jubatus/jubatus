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

#include "inserting_stage.hpp"

#include <iostream>
#include <string>
#include <gtest/gtest.h>

#include "stage_test_tool.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

TEST(inserting_stage, general) {
  inserting_stage* target_stage = new inserting_stage(kWindowBatchSize,
                                                      kBatchInterval);
  dummy_out_stage* o_stage = new dummy_out_stage();
  target_stage->set_next(o_stage);
  double default_weight_arr[] = {kDefaultBatchWeight,
                                 kDefaultBatchWeight,
                                 kDefaultBatchWeight,
                                 kDefaultBatchWeight,
                                 kDefaultBatchWeight};

  // ------
  // start pos = 1950
  // end pos = 2000
  // batch size = 5
  // batch interval = 10
  // ------

  // batch id = 4
  ASSERT_EQ(kPosIsInWindow,
            target_stage->add_document(k_dArr[4], k_rArr[4], kPos));
  window* next_window4 = o_stage->get_window();
  checkWindowIsDroppedInThisStage(next_window4);
  o_stage->reset_window();

  // batch id = 0
  ASSERT_EQ(kPosIsInWindow,
            target_stage->add_document(k_dArr[0], k_rArr[0], 1957.1));
  window* next_window0 = o_stage->get_window();
  checkWindowIsDroppedInThisStage(next_window0);
  o_stage->reset_window();

  // batch id = 1
  ASSERT_EQ(kPosIsInWindow,
            target_stage->add_document(k_dArr[1], k_rArr[1], 1962.4));
  window* next_window1 = o_stage->get_window();
  checkWindowIsDroppedInThisStage(next_window1);
  o_stage->reset_window();

  // batch id = 2
  ASSERT_EQ(kPosIsInWindow,
            target_stage->add_document(k_dArr[2], k_rArr[2], 1979.9));
  window* next_window2 = o_stage->get_window();
  checkWindowIsDroppedInThisStage(next_window2);
  o_stage->reset_window();

  // batch id = 3
  ASSERT_EQ(kPosIsInWindow,
            target_stage->add_document(k_dArr[3], k_rArr[3], 1980.0));
  window* next_window3 = o_stage->get_window();
  checkWindowIsDroppedInThisStage(next_window3);
  o_stage->reset_window();

  // older pos
  ASSERT_EQ(kPosIsOlderThanWindow,
            target_stage->add_document(1, 0, 1949.9));
  window* next_window5 = o_stage->get_window();
  checkWindowIsDroppedInThisStage(next_window5);
  o_stage->reset_window();

  // newer pos -> push
  ASSERT_EQ(kPosIsNewerThanWindow,
            target_stage->add_document(1, 0, 2000));
  window* next_window6 = o_stage->get_window();

  checkWindowIsPushedToNextStage(k_dArr, k_rArr, default_weight_arr,
                                 kWindowBatchSize,
                                 kBatchInterval,
                                 next_window6);

  delete_window_ptr(next_window0);
  delete_window_ptr(next_window1);
  delete_window_ptr(next_window2);
  delete_window_ptr(next_window3);
  delete_window_ptr(next_window4);
  delete_window_ptr(next_window5);
  delete_window_ptr(next_window6);

  delete target_stage;
  delete o_stage;
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
