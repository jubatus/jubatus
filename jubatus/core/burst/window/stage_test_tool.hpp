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

#ifndef JUBATUS_CORE_BURST_WINDOW_STAGE_TEST_TOOL_HPP_
#define JUBATUS_CORE_BURST_WINDOW_STAGE_TEST_TOOL_HPP_

#include <cfloat>
#include <iostream>
#include <vector>
#include <gtest/gtest.h>

#include "stage_base.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

// const int kWindowBatchSize = 3;
const int kWindowBatchSize = 5;
const int kBatchInterval = 10;
const double kScalingParam = 2;
const double kGamma = 1;
const double kCostcutThreshold = DBL_MAX;
const int kMaxReuseBatchSize = 3;

const double kPos = 1991.5;
const int kStartPos = 1950;


const uint32_t k_dArr[] = {10, 12, 4, 5, 8};
const uint32_t k_rArr[] = {1, 2, 3, 2, 0};
const double kWeightArr[] = {0, 2.2, 3.5, 1.2, 100.4};

const double kUnittestSignificantDigit = 0.0001;

class dummy_in_stage : public stage_base {
 public:
  ~dummy_in_stage() {
    // delete next_;
  }

  void go(const uint32_t d_arr[],
          const uint32_t r_arr[],
          const double weight_arr[]) {
    window* w = create_window_ptr(kPos, kWindowBatchSize, kBatchInterval);
    double pos = w->get_start_pos();
    std::vector<double> w_vec;
    for (int i = 0; i < kWindowBatchSize; i++) {
      w->add_document(d_arr[i], r_arr[i], pos);
      w_vec.push_back(weight_arr[i]);
      pos += kBatchInterval;
    }
    w->set_batch_weights(w_vec);
    next_->push(w);
  }
};

class dummy_out_stage : public stage_base {
 public:
  dummy_out_stage() : window_(0) {
  }
  ~dummy_out_stage() {
    // delete next_;
  }

  void reset_window() {
    window_ = 0;
  }

  window* get_window() {
    return window_;
  }

  const window* get_copied() const {
    if (window_ == 0) {
      return 0;
    }
    window* ret = create_window_ptr(*window_);
    return ret;
  }

 private:
  bool is_drop(const window* w) const {
    return false;
  }
  void do_something(window* w) {
    std::cout << "pushed to next stage" << std::endl;
    delete_window_ptr(window_);
    window_ = w;
  }
  void go_to_next_stage(window* w) {
  }

  window* window_;
};

inline void checkWindowIsDroppedInThisStage(const window* next_window) {
  std::cout << "* checkWindowIsDroppedInThisStage" << std::endl;
  ASSERT_EQ(0, next_window);
}

inline void checkWindowIsPushedToNextStage(const uint32_t d_arr[],
                                           const uint32_t r_arr[],
                                           const double weight_arr[],
                                           const int window_batch_size,
                                           int batch_interval,
                                           const window* next_window) {
  std::cout << "* checkWindowIsPushedToNextStage" << std::endl;

  ASSERT_TRUE(next_window);
  ASSERT_EQ(window_batch_size, next_window->get_batch_size());
  ASSERT_EQ(batch_interval, next_window->get_batch_interval());
  ASSERT_EQ(kStartPos, next_window->get_start_pos());

  const std::vector<batch>& batches = next_window->get_batches();
  for (int i = 0; i < window_batch_size; i++) {
    ASSERT_EQ(d_arr[i], batches[i].get_d());
    ASSERT_EQ(r_arr[i], batches[i].get_r());
    ASSERT_NEAR(weight_arr[i], batches[i].get_batch_weight(),
                kUnittestSignificantDigit);
  }
}

inline void checkGetResult(const uint32_t d_arr[],
                           const uint32_t r_arr[],
                           const double weight_arr[],
                           int window_batch_size,
                           int batch_interval,
                           int start_pos,
                           int end_pos,
                           const window& get_result_window) {
  std::cout << "* checkGetResult" << std::endl;

  ASSERT_EQ(window_batch_size, get_result_window.get_batch_size());
  ASSERT_EQ(batch_interval, get_result_window.get_batch_interval());
  ASSERT_EQ(start_pos, get_result_window.get_start_pos());
  ASSERT_EQ(end_pos, get_result_window.get_end_pos());

  const std::vector<batch>& batches = get_result_window.get_batches();
  for (int i = 0; i < window_batch_size; i++) {
    ASSERT_EQ(d_arr[i], batches[i].get_d());
    ASSERT_EQ(r_arr[i], batches[i].get_r());
    ASSERT_NEAR(weight_arr[i], batches[i].get_batch_weight(),
                kUnittestSignificantDigit);
  }
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_WINDOW_STAGE_TEST_TOOL_HPP_
