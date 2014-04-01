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

#ifndef JUBATUS_CORE_BURST_WINDOW_PROCEEDED_STAGE_HPP_
#define JUBATUS_CORE_BURST_WINDOW_PROCEEDED_STAGE_HPP_

#include <deque>
#include "stage_base.hpp"
#include "window.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

/**
 * the stage after the inserting/calculating_stage
 */
class proceeded_stage : public stage_base {
 public:
  /**
   * constructor.
   * @param rotate_size
   *        the number of windows to keep
   */
  explicit proceeded_stage(const int32_t rotate_size)
      : rotate_size_(rotate_size) {
  }

  ~proceeded_stage();

  /**
   * get newest calculated "copied" window
   * @return copy of window
   */
  const window* get_copied() const;
  window get_result() const;
  window get_result_at(const double& pos) const;

 private:
  /**
   * the function to return whether or not
   * the window is dropped in this stage.
   * @param w the pointer of window
   * @return if coming window is newest  -> false,
   *         else                        -> true
   */
  bool is_drop(const window* w) const;

  /**
   * add window to queue
   * @param w the pointer of window
   */
  void do_something(window* w);

  std::deque<window*> window_list_;
  const int32_t rotate_size_;
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_WINDOW_PROCEEDED_STAGE_HPP_
