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

#ifndef JUBATUS_CORE_BURST_WINDOW_CALCULATING_STAGE_HPP_
#define JUBATUS_CORE_BURST_WINDOW_CALCULATING_STAGE_HPP_

#include "jubatus/util/concurrent/lock.h"
#include "jubatus/util/lang/bind.h"
#include "jubatus/util/concurrent/condition.h"
#include "jubatus/util/concurrent/rwmutex.h"
#include "jubatus/util/concurrent/mutex.h"
#include "jubatus/util/concurrent/thread.h"

#include "stage_base.hpp"
#include "window.hpp"
#include "../engine/burst_detect.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

/**
 * the stage burst detection is executed.
 */
class calculating_stage : public stage_base {
 public:
  /**
   * constructor.
   * @param scaling_param
   *        Kleinberg paper's "scaling parameter"
   * @param gamma
   *        Kleinberg paper's "gamma"
   * @param costcut_threshold
   *        costcut threshold in burst detect.
   * @param max_reuse_batch_num
   *        the max number of batches to reuse the batch weight.
   */
  calculating_stage(double scaling_param,
                    double gamma,
                    double costcut_threshold,
                    int32_t max_reuse_batch_num)
    : scaling_param_(scaling_param),
      gamma_(gamma),
      costcut_threshold_(costcut_threshold),
      max_reuse_batch_num_(max_reuse_batch_num),
      t_(jubatus::util::lang::bind(&calculating_stage::do_something_impl,
                                   this)),
      window_(0) {
  }

  ~calculating_stage();

  double get_scaling_param() {
    return scaling_param_;
  }

  double get_gamma() {
    return gamma_;
  }

  jubatus::util::concurrent::rw_mutex& get_mutex() {
    return m_;
  }

  jubatus::util::concurrent::thread& get_thread() {
    return t_;
  }

 private:
  /**
   * the function to return whether or not
   * the window is dropped in this stage.
   * @param w the pointer of window
   * @return this burst detect process is running -> true, else -> false.
   */
  bool is_drop(const window* w) const;

  /**
   * the function to execute to the window
   * when the is_drop() function passed.
   * @param w the pointer of window
   */
  void do_something(window* w);

  /**
   * threading made in do_something.
   * @param w the pointer of window
   */
  // void do_something_impl(window* w);
  void do_something_impl();

  /**
   * empty operation.
   * @param w the pointer of window.
   */
  void go_to_next_stage(window* w);

  const double scaling_param_;

  const double gamma_;

  const double costcut_threshold_;

  const int32_t max_reuse_batch_num_;

  /**
   * lock object.
   */
  mutable jubatus::util::concurrent::rw_mutex m_;

  jubatus::util::concurrent::thread t_;

  window* window_;
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_WINDOW_CALCULATING_STAGE_HPP_
