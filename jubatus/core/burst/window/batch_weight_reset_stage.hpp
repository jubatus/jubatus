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

#ifndef JUBATUS_CORE_BURST_WINDOW_BATCH_WEIGHT_RESET_STAGE_HPP_
#define JUBATUS_CORE_BURST_WINDOW_BATCH_WEIGHT_RESET_STAGE_HPP_

#include <iostream>
#include "stage_base.hpp"
#include "window_helper.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

class batch_weight_reset_stage : public stage_base {
 private:
  /**
   * the function to return whether or not
   * the window is dropped in this stage.
   * in this stage, the window is always passed.
   * @param w the pointer of window
   * @return false
   */
  bool is_drop(const window* w) const;

  /**
   * initialize batch_weight.
   * @param w the pointer of window
   */
  void do_something(window* w);
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_WINDOW_BATCH_WEIGHT_RESET_STAGE_HPP_
