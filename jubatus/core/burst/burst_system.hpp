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

#ifndef JUBATUS_CORE_BURST_BURST_SYSTEM_HPP_
#define JUBATUS_CORE_BURST_BURST_SYSTEM_HPP_

#include <vector>
#include <list>
#include <map>
#include <string>

#include "window/batch.hpp"
#include "window/batch_weight_reset_stage.hpp"
#include "window/calculating_stage.hpp"
#include "window/inserting_stage.hpp"
#include "window/keyword_filter_stage.hpp"
#include "window/proceeded_stage.hpp"
#include "window/stage_base.hpp"
#include "window/waiting_mix_stage.hpp"
#include "window/window.hpp"
#include "jubatus/server/server/burst_types.hpp"
#include "jubatus/util/lang/noncopyable.h"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

/**
 * burst system class.
 * one burst_system is created for one keyword.
 */
class burst_system : jubatus::util::lang::noncopyable {
 public:
  /**
   * constructor
   * @param keyword_str
   *        keyword of burst_system.
   * @param window_batch_size
   *        batches size of window.
   * @param batch_interval
   *        interval of one batch.
   * @param scaling_param
   *        Kleinberg paper's "scaling parameter"
   * @param gamma
   *        Kleinberg paper's "gamma"
   * @param costcut_threshold
   *        costcut threshold in burst detect.
   * @param max_reuse_batch_num
   *        the max number of batches to reuse the batch weight.
   * @param result_window_rotate_size
   *        the number of calculated windows to keep
   */
  burst_system(const std::string& keyword_str,
               int32_t window_batch_size,
               int32_t batch_interval,
               double scaling_param,
               double gamma,
               double costcut_threshold,
               int32_t max_reuse_batch_num,
               int32_t result_window_rotate_size);

  ~burst_system();

  bool add_document(const std::string& doc_txt, double pos);

  window get_result() const;

  window get_result_at(double pos) const;

  std::string get_keyword_str() const {
    return keyword_str_;
  }

  double get_scaling_param() const;
  double get_gamma() const;

  window get_diff_impl() const;
  void put_diff_impl(const window& w);

  jubatus::util::concurrent::rw_mutex& get_calculating_mutex() {
    return calculating_stage_->get_mutex();
  }

  jubatus::util::concurrent::thread& get_calculating_thread() {
    return calculating_stage_->get_thread();
  }

 private:
  /**
   * the stage of document insert.
   * the next stage is inserted_stage_.
   */
  inserting_stage* inserting_stage_;

  /**
   * the stage of after-document insert.
   * the next stage is before_calculate_filter_stage_.
   */
  proceeded_stage* inserted_stage_;

  /**
   * the stage of keyword_filter.
   * the next stage is calculating_stage_.
   */
  keyword_filter_stage* before_calculate_filter_stage_;

  /**
   * the stage of burst detect.
   * the next stage is calculated_stage_.
   */
  calculating_stage* calculating_stage_;

  /**
   * the stage of after burst detect.
   * the next stage is before_waiting_mix_filter_stage_.
   */
  proceeded_stage* calculated_stage_;

  /**
   * the stage of keyword_filter.
   * the next stage is waiting_mix_stage_.
   */
  keyword_filter_stage* before_waiting_mix_filter_stage_;

  /**
   * the stage of waiting mix.
   */
  waiting_mix_stage* waiting_mix_stage_;

  /**
   * the stage of reset batch_weight.
   * the next stage is inserted_stage_.
   */
  batch_weight_reset_stage* batch_weight_reset_stage_;

  std::string keyword_str_;
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_BURST_SYSTEM_HPP_
