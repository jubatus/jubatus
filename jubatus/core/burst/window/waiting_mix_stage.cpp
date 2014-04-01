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

#include <glog/logging.h>

namespace jubatus {
namespace core {
namespace burst {
namespace system {

waiting_mix_stage::~waiting_mix_stage() {
  while (static_cast<int32_t>(window_list_.size()) != 0) {
    window* w_poped = window_list_.front();
    window_list_.pop_front();
    delete_window_ptr(w_poped);
  }
}

const window* waiting_mix_stage::get_diff() {
  DLOG(INFO) << "get_diff: start";
  if (window_list_.size() == 0) {
    DLOG(INFO) << "get_diff: window_list_.size() == 0 -> return 0";
    return 0;
  }
  window w = *(window_list_.back());
  window* ret = create_window_ptr(w);

  while (window_list_.size() != 0) {
    window* w_poped = window_list_.front();
    window_list_.pop_front();
    delete_window_ptr(w_poped);
  }
  return ret;
}

bool waiting_mix_stage::is_drop(const window* w) const {
  return false;
}

void waiting_mix_stage::do_something(window* w) {
  window_list_.push_back(w);
  if (rotate_size_ < static_cast<int32_t>(window_list_.size())) {
    window* w_poped = window_list_.front();
    window_list_.pop_front();
    delete_window_ptr(w_poped);
  }
}

void waiting_mix_stage::go_to_next_stage(window* w) {
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
