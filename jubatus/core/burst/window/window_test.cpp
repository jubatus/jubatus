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


#include <limits.h>
#include <iostream>
#include <vector>

#include <gtest/gtest.h>

#include "window.hpp"

TEST(window, general) {
  //
  // test1. positive pos.
  //
  jubatus::core::burst::system::window w0(1245.88, 30, 5);

  // constructor test
  ASSERT_EQ(30, static_cast<int>(w0.get_batches().size()));
  ASSERT_EQ(1100, w0.get_start_pos());
  ASSERT_EQ(1250, w0.get_end_pos());
  for (int i = 0; i < 30; i++) {
    ASSERT_EQ(static_cast<uint32_t>(0),
              w0.get_batches().at(i).get_d());
    ASSERT_EQ(static_cast<uint32_t>(0),
              w0.get_batches().at(i).get_r());
    ASSERT_EQ(-1.0,
              w0.get_batches().at(i).get_batch_weight());
  }

  // add_document test
  ASSERT_EQ(jubatus::core::burst::system::kPosIsOlderThanWindow,
                       w0.add_document(10, 3, 567.8));  // older data
  for (int i = 0; i < 30; i++) {
    ASSERT_EQ(static_cast<uint32_t>(0),
              w0.get_batches().at(i).get_d());
    ASSERT_EQ(static_cast<uint32_t>(0),
              w0.get_batches().at(i).get_r());
    ASSERT_EQ(-1.0, w0.get_batches().at(i).get_batch_weight());
  }
  ASSERT_EQ(jubatus::core::burst::system::kPosIsNewerThanWindow,
            w0.add_document(10, 3, 1320));  // newer data
  for (int i = 0; i < 30; i++) {
    ASSERT_EQ(static_cast<uint32_t>(0),
              w0.get_batches().at(i).get_d());
    ASSERT_EQ(static_cast<uint32_t>(0),
              w0.get_batches().at(i).get_r());
    ASSERT_EQ(-1.0, w0.get_batches().at(i).get_batch_weight());
  }
  ASSERT_EQ(jubatus::core::burst::system::kPosIsInWindow,
            w0.add_document(10, 3, 1162.5));
  for (int i = 0; i < 30; i++) {
    if (i == 12) {
      ASSERT_EQ(static_cast<uint32_t>(10),
                w0.get_batches().at(i).get_d());
      ASSERT_EQ(static_cast<uint32_t>(3),
                w0.get_batches().at(i).get_r());
    } else {
      ASSERT_EQ(static_cast<uint32_t>(0),
                w0.get_batches().at(i).get_d());
      ASSERT_EQ(static_cast<uint32_t>(0),
                w0.get_batches().at(i).get_r());
    }
    ASSERT_EQ(-1.0, w0.get_batches().at(i).get_batch_weight());
  }

  // copy constructor test
  std::vector<double> batch_weights;
  for (int i = 0; i < 30; i++) {
    batch_weights.push_back(i*i + 0.5);
  }
  w0.set_batch_weights(batch_weights);

  jubatus::core::burst::system::window w1(w0);
  ASSERT_EQ(30, static_cast<int>(w1.get_batches().size()));
  ASSERT_EQ(1100, w1.get_start_pos());
  ASSERT_EQ(1250, w1.get_end_pos());
  for (int i = 0; i < 30; i++) {
    if (i == 12) {
      ASSERT_EQ(static_cast<uint32_t>(10),
                w1.get_batches().at(i).get_d());
      ASSERT_EQ(static_cast<uint32_t>(3),
                w1.get_batches().at(i).get_r());
    } else {
      ASSERT_EQ(static_cast<uint32_t>(0),
                w1.get_batches().at(i).get_d());
      ASSERT_EQ(static_cast<uint32_t>(0),
                w1.get_batches().at(i).get_r());
    }
    ASSERT_EQ(batch_weights.at(i),
              w1.get_batches().at(i).get_batch_weight());
  }

  // copy_d_r test
  jubatus::core::burst::system::window w2(1000, 30, 5);
  ASSERT_EQ(0, w2.copy_d_r(w0));

  jubatus::core::burst::system::window w3(1298, 30, 5);
  ASSERT_TRUE(w3.copy_d_r(w0));
  for (int i = 0; i < 30; i++) {
    if (i == 2) {
      ASSERT_EQ(static_cast<uint32_t>(10),
                w3.get_batches().at(i).get_d());
      ASSERT_EQ(static_cast<uint32_t>(3),
                w3.get_batches().at(i).get_r());
    } else {
      ASSERT_EQ(static_cast<uint32_t>(0),
                w3.get_batches().at(i).get_d());
      ASSERT_EQ(static_cast<uint32_t>(0),
                w3.get_batches().at(i).get_r());
    }
    ASSERT_EQ(-1.0, w3.get_batches().at(i).get_batch_weight());
  }

  // copy_batch_weights test
  jubatus::core::burst::system::window w4(1000, 30, 5);
  ASSERT_EQ(0, w4.copy_batch_weight(w0, INT_MAX));

  jubatus::core::burst::system::window w5(1295.5, 30, 5);
  ASSERT_TRUE(w5.copy_batch_weight(w0, INT_MAX));
  for (int i = 0; i < 30; i++) {
    ASSERT_EQ(static_cast<uint32_t>(0),
              w5.get_batches().at(i).get_d());
    ASSERT_EQ(static_cast<uint32_t>(0),
              w5.get_batches().at(i).get_r());
    if (i < 20) {
      ASSERT_EQ(batch_weights.at(i + 10),
                w5.get_batches().at(i).get_batch_weight());
    } else {
      ASSERT_EQ(-1.0, w5.get_batches().at(i).get_batch_weight());
    }
  }

  //
  // test2. negative pos.
  //
  jubatus::core::burst::system::window w6(-100.5, 20, 5);
  // constructor test
  ASSERT_EQ(20, static_cast<int>(w6.get_batches().size()));
  ASSERT_EQ(-200, w6.get_start_pos());
  ASSERT_EQ(-100, w6.get_end_pos());
  for (int i = 0; i < 20; i++) {
    ASSERT_EQ(static_cast<uint32_t>(0),
              w6.get_batches().at(i).get_d());
    ASSERT_EQ(static_cast<uint32_t>(0),
              w6.get_batches().at(i).get_r());
    ASSERT_EQ(-1.0, w6.get_batches().at(i).get_batch_weight());
  }
}
