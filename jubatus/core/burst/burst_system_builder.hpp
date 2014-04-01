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

#ifndef JUBATUS_CORE_BURST_BURST_SYSTEM_BUILDER_HPP_
#define JUBATUS_CORE_BURST_BURST_SYSTEM_BUILDER_HPP_

#include <string>

#include "burst_system.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

class burst_system_builder {
 public:
  /**
   * constructor
   * @param window_batch_size
   *        batches size of window.
   * @param batch_interval
   *        interval of one batch.
   * @param costcut_threshold
   *        costcut threshold in burst detect.
   * @param max_reuse_batch_num
   *        the max number of batches to reuse the batch weight.
   * @param result_window_rotate_size
   *        the number of calculated windows to keep
   */
  burst_system_builder(int32_t window_batch_size,
                       int32_t batch_interval,
                       double costcut_threshold,
                       int32_t max_reuse_batch_num,
                       int32_t result_window_rotate_size)
      : window_batch_size_(window_batch_size),
        batch_interval_(batch_interval),
        costcut_threshold_(costcut_threshold),
        max_reuse_batch_num_(max_reuse_batch_num),
        result_window_rotate_size_(result_window_rotate_size) {
  }

  /**
   * build function
   * @param keyword_txt
   *        keyword of burst_system.
   * @param scaling_param
   *        Kleinberg paper's "scaling parameter"
   * @param gamma
   *        Kleinberg paper's "gamma"
   * @return the pointer of burst_system
   */
  burst_system* build(const std::string& keyword_txt,
                      double scaling_param,
                      double gamma);

  void delete_ptr(burst_system* system);

 private:
  const int32_t window_batch_size_;
  const int32_t batch_interval_;
  const double costcut_threshold_;
  const int32_t max_reuse_batch_num_;
  const int32_t result_window_rotate_size_;
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_BURST_SYSTEM_BUILDER_HPP_
