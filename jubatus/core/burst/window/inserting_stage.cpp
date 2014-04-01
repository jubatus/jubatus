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

#include <iostream>
#include "inserting_stage.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {


inserting_stage::~inserting_stage() {
  delete_window_ptr(window_);
}

AddDocumentResults inserting_stage::add_document(const int32_t d,
                                                 const int32_t r,
                                                 const double& pos) {
  if (window_ == 0) {
    create_window(pos);
    return window_->add_document(d, r, pos);
  }

  if (window_->is_pos_older_than_me(pos)) {
    return kPosIsOlderThanWindow;
  } else if (window_->is_pos_newer_than_me(pos)) {
    next_->push(window_);
    create_window(pos);
    window_->add_document(d, r, pos);
    const window* w_copied = next_->get_copied();
    window_->copy_d_r(*w_copied);
    delete_window_ptr(w_copied);
    return kPosIsNewerThanWindow;
  }
  return window_->add_document(d, r, pos);
}

void inserting_stage::create_window(const double& pos) {
  window_ = create_window_ptr(pos, window_batch_size_, batch_interval_);
}

bool inserting_stage::is_drop(const window*) const {
  return false;
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
