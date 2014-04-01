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

#include "keyword_filter_stage.hpp"

#include <iostream>
#include <gtest/gtest.h>
#include "stage_test_tool.hpp"

using jubatus::util::lang::shared_ptr;
using jubatus::core::burst::util::my_keyword_list;

namespace jubatus {
namespace core {
namespace burst {
namespace system {

TEST(keyword_filter_stage, general) {
  dummy_in_stage* i_stage = new dummy_in_stage();
  keyword_filter_stage* target_stage = new keyword_filter_stage("keyword");
  dummy_out_stage* o_stage = new dummy_out_stage();
  i_stage->set_next(target_stage);
  target_stage->set_next(o_stage);
  window* next_window;

  shared_ptr<my_keyword_list> k_list = my_keyword_list::get_instance();

  std::cout << "test 1: not push to next" << std::endl;
  i_stage->go(k_dArr, k_rArr, kWeightArr);
  next_window = o_stage->get_window();
  checkWindowIsDroppedInThisStage(next_window);

  delete_window_ptr(next_window);

  std::cout << "test 2: push to next" << std::endl;
  k_list->add_keyword("keyword");
  k_list->add_keyword("keyword1");
  k_list->add_keyword("keyword2");
  i_stage->go(k_dArr, k_rArr, kWeightArr);
  next_window = o_stage->get_window();
  checkWindowIsPushedToNextStage(k_dArr, k_rArr, kWeightArr,
                                 kWindowBatchSize,
                                 kBatchInterval,
                                 next_window);

  delete_window_ptr(next_window);
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
