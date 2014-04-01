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

#include <iostream>
#include <vector>

#include <glog/logging.h>
#include "jubatus/util/system/time_util.h"

using jubatus::util::system::time::clock_time;
using jubatus::util::system::time::get_clock_time;

namespace jubatus {
namespace core {
namespace burst {
namespace system {

calculating_stage::~calculating_stage() {
  delete_window_ptr(window_);
}

bool calculating_stage::is_drop(const window* w) const {
  bool is_drop = !(m_.write_lock());
  if (is_drop) {
    DLOG(INFO) << "calculating_stage: burst detect is skipped.";
  }
  return is_drop;
}

void calculating_stage::do_something(window* w) {
  window_ = w;
  t_.join();
  t_.start();
}

void calculating_stage::do_something_impl() {
  clock_time start = get_clock_time();

  std::vector<double> batch_weight_vec = window_->get_batch_weights();
  const window* w_copied = next_->get_copied();
  if (w_copied != 0) {
    if (window_->get_start_pos() < w_copied->get_start_pos()) {
      delete_window_ptr(window_);
      delete_window_ptr(w_copied);
      m_.unlock();
      return;
    }
    window_->copy_batch_weight(*w_copied,
                               max_reuse_batch_num_);
  }
  delete_window_ptr(w_copied);
  engine::burst_detect(
      window_->get_d_vec(),
      window_->get_r_vec(),
      batch_weight_vec,
      scaling_param_,
      gamma_,
      costcut_threshold_);
  window_->set_batch_weights(batch_weight_vec);

  clock_time end = get_clock_time();
  DLOG(INFO) << "burst_detect done. in "
             << static_cast<double>(end - start)
             << " secs. ";

  next_->push(window_);
  window_ = 0;
  m_.unlock();
}

void calculating_stage::go_to_next_stage(window* w) {
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
