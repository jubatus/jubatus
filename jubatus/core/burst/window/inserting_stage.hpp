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

#ifndef JUBATUS_CORE_BURST_WINDOW_INSERTING_STAGE_HPP_
#define JUBATUS_CORE_BURST_WINDOW_INSERTING_STAGE_HPP_

#include "stage_base.hpp"
#include "window.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

/**
 * the stage add_document is executed.
 */
class inserting_stage : public stage_base {
 public:
  /**
   * constructor.
   * @param window_batch_size
   *        batches size of window.
   * @param batch_interval
   *        interval of one batch.
   */
  inserting_stage(int32_t window_batch_size,
                  int32_t batch_interval)
    : window_(0),
      window_batch_size_(window_batch_size),
      batch_interval_(batch_interval) {
  }

  ~inserting_stage();

  /**
   * add d and r of document data.
   * @param d d
   * @param r r
   * @param pos
   *        the position of document.
   * @return if pos is older than window      -> kPosIsOlderThanWindow,
   *         else if pos is newer than window -> kPosIsNewerThanWindow,
   *         else                             -> kPosIsInWindow
   */
  AddDocumentResults add_document(const int32_t d,
                                  const int32_t r,
                                  const double& pos);

 private:
  /**
   * create new window.
   * @param pos position
   */
  void create_window(const double& pos);

  /**
   * (not use)
   * @param w the pointer of window
   * @return dropped -> true, passed -> false
   */
  bool is_drop(const window* w) const;

  window* window_;
  int32_t window_batch_size_;
  int32_t batch_interval_;
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_WINDOW_INSERTING_STAGE_HPP_
