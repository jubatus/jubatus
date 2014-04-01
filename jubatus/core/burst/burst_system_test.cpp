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

#include "burst_system.hpp"

#include <iostream>
#include <gtest/gtest.h>
#include "my_keyword_list.hpp"
#include "window/stage_test_tool.hpp"

using jubatus::util::lang::shared_ptr;
namespace jubatus {
namespace core {
namespace burst {
namespace system {

TEST(burst_system, general) {
  burst_system b_system("keyword1",
                        kWindowBatchSize,
                        kBatchInterval,
                        kScalingParam,
                        kGamma,
                        kCostcutThreshold,
                        kMaxReuseBatchSize,
                        10);

  /*
   * add_keyword
   */
  shared_ptr<util::my_keyword_list> k_list =
      util::my_keyword_list::get_instance();
  k_list->add_keyword("keyword1");

  // no document added.
  // ret "default" window.
  // batch size = 0
  window w0 = b_system.get_result();
  ASSERT_EQ(0, w0.get_batch_size());

  // create test data
  // - d = (10,10,10,10,10)
  // - r = ( 0, 0, 0, 0,10)
  for (int i = 0; i < 10; i++) {
    // add (1,1)
    ASSERT_TRUE(b_system.add_document("keyword1", 1998.5));
    // add (1,0)
    ASSERT_TRUE(b_system.add_document("keyword2", 1958.5));
    ASSERT_TRUE(b_system.add_document("keyword2", 1968.5));
    ASSERT_TRUE(b_system.add_document("keyword2", 1978.5));
    ASSERT_TRUE(b_system.add_document("keyword2", 1988.5));
  }

  // older pos
  ASSERT_FALSE(b_system.add_document("keyword2", 1500.5));


  // slide 1 batch
  // test data
  // - d = (10,10,10,10,10)
  // - r = ( 0, 0, 0,10,10)
  for (int i = 0; i < 10; i++) {
    // add (1,1)
    ASSERT_TRUE(b_system.add_document("keyword1", 2000));
  }

  sleep(2);

  window w1 = b_system.get_result();

  const uint32_t d_arr1[] = {10, 10, 10, 10, 10};
  const uint32_t r_arr1[] = {0, 0, 0, 0, 10};
  const double weight_arr1[] = {0.0, 0.0, 0.0, 0.0, 6.931472};

  checkGetResult(d_arr1, r_arr1, weight_arr1,
                 kWindowBatchSize,
                 kBatchInterval,
                 1950, 2000,
                 w1);

  // slide 1 batch
  // test data
  // - d = (10,10,10,10,10)
  // - r = ( 0, 0,10,10, 0)
  for (int i = 0; i < 10; i++) {
    // add (1,0)
    ASSERT_TRUE(b_system.add_document("keyword2", 2010));
  }

  sleep(2);

  window w2 = b_system.get_result();

  const uint32_t d_arr2[] = {10, 10, 10, 10, 10};
  const uint32_t r_arr2[] = {0, 0, 0, 10, 10};
  const double weight_arr2[] = {0.0, 0.0, 0.0, 6.931472, 6.931472};

  checkGetResult(d_arr2, r_arr2, weight_arr2,
                 kWindowBatchSize,
                 kBatchInterval,
                 1960, 2010,
                 w2);

  // slide 2 batch
  // test data
  // - d = (10,10,10, 0, 20)
  // - r = (10,10, 0, 0,  0)
  for (int i = 0; i < 20; i++) {
    // add (1,0)
    ASSERT_TRUE(b_system.add_document("keyword2", 2030));
  }

  sleep(2);

  ASSERT_TRUE(b_system.add_document("keyword2", 2040));

  sleep(2);


  window w3 = b_system.get_result();

  const uint32_t d_arr3[] = {10, 10, 10,  0, 20};
  const uint32_t r_arr3[] = {10, 10,  0,  0,  0};
  const double weight_arr3[] = {6.931472, 6.931472, 0.0, 0.0, 0.0};

  checkGetResult(d_arr3, r_arr3, weight_arr3,
                 kWindowBatchSize,
                 kBatchInterval,
                 1990, 2040,
                 w3);

  window w4 = b_system.get_result_at(1955);
  checkGetResult(d_arr1, r_arr1, weight_arr1,
                 kWindowBatchSize,
                 kBatchInterval,
                 1950, 2000,
                 w4);


  burst_system b_system1("keyword1",
                         kWindowBatchSize,
                         kBatchInterval,
                         kScalingParam,
                         kGamma,
                         kCostcutThreshold,
                         kMaxReuseBatchSize,
                         10);


  /*
   * test2
   * window slide
   */

  for (int i = 0; i < 10; i++) {
    // add (1,0)
    ASSERT_TRUE(b_system1.add_document("", 0));
  }
  sleep(2);
  window w1_1 = b_system1.get_result();
  // std::cout << w1_1.get_start_pos() << std::endl;



  for (int i = 0; i < 10; i++) {
    // add (1,0)
    ASSERT_TRUE(b_system1.add_document("", 5));
  }
  sleep(2);
  window w1_2 = b_system1.get_result();
  // std::cout << w1_2.get_start_pos() << std::endl;

  for (int i = 0; i < 10; i++) {
    // add (1,0)
    ASSERT_TRUE(b_system1.add_document("", 10));
  }
  sleep(2);
  window w1_3 = b_system1.get_result();

  for (int i = 0; i < 10; i++) {
    // add (1,0)
    ASSERT_TRUE(b_system1.add_document("", 15));
  }
  sleep(2);
  window w1_4 = b_system1.get_result();
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
