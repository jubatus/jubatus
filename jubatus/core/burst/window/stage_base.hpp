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

#ifndef JUBATUS_CORE_BURST_WINDOW_STAGE_BASE_HPP_
#define JUBATUS_CORE_BURST_WINDOW_STAGE_BASE_HPP_

#include "window_helper.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

/**
 * the base class of stage.
 */
class stage_base {
 public:
  /**
   * set next stage pointer.
   * @param next
   *        next stage pointer
   */
  void set_next(stage_base* next);

  /**
   * push the window to this stage.
   * @param w the pointer of window
   */
  void push(window* w);

  /**
   * get the copy pointer.
   * @return the pointer of window
   */
  const virtual window* get_copied() const;

  virtual ~stage_base() {}

 protected:
  /**
   * the function to return whether or not
   * the window is dropped in this stage.
   * @param w the pointer of window
   * @return dropped -> true, passed -> false
   */
  virtual bool is_drop(const window* w) const {
    return true;
  }

  /**
   * the function to execute to the window
   * when the is_drop() function passed.
   * @param w the pointer of window
   */
  virtual void do_something(window* w) {
  }

  /**
   * send the window to the next stage.
   * @param w the pointer of window.
   */
  virtual void go_to_next_stage(window* w);

  /**
   * destroy the window
   * @param w the pointer of window.
   */
  virtual void destroy_window(window* w);

  stage_base* next_;
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_WINDOW_STAGE_BASE_HPP_
