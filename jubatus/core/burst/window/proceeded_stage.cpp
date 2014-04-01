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

#include <deque>
#include <iostream>
#include <glog/logging.h>

#include "proceeded_stage.hpp"


namespace jubatus {
namespace core {
namespace burst {
namespace system {

proceeded_stage::~proceeded_stage() {
  while (!window_list_.empty()) {
    window* w_poped = window_list_.front();
    window_list_.pop_front();
    delete_window_ptr(w_poped);
  }
}

const window* proceeded_stage::get_copied() const {
  if (window_list_.size() == 0) {
    return 0;
  }
  window* ret = create_window_ptr(*window_list_.back());
  return ret;
}

window proceeded_stage::get_result() const {
  if (window_list_.size() == 0) {
    return window();
  }
  window ret(*window_list_.back());
  return ret;
}

window proceeded_stage::get_result_at(const double& pos) const {
  std::deque<window*>::const_reverse_iterator rit = window_list_.rbegin();
  for (rit = window_list_.rbegin(); rit != window_list_.rend(); ++rit) {
    if ((*rit)->is_pos_in_me(pos)) {
      return window(**rit);
    }
  }
  return window();
}


bool proceeded_stage::is_drop(const window* w) const {
  DLOG(INFO) << "proceeded_stage::is_drop: start, "
             << "window_list_.size() = " << window_list_.size();
  int32_t w_start_pos = w->get_start_pos();
  if ((0 < window_list_.size())
      && (w_start_pos <= window_list_.back()->get_start_pos())) {
    return true;
  }
  return false;
}

void proceeded_stage::do_something(window* w) {
  window* w1 = create_window_ptr(*w);
  window_list_.push_back(w1);
  if (rotate_size_ < static_cast<int32_t>(window_list_.size())) {
    window* w_poped = window_list_.front();
    window_list_.pop_front();
    delete_window_ptr(w_poped);
  }
}


}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
