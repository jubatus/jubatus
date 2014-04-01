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

#include "burst_system.hpp"

#include <string>
#include <glog/logging.h>

#include "window/window.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

  burst_system::burst_system(const std::string& keyword_str,
                             int32_t window_batch_size,
                             int32_t batch_interval,
                             double scaling_param,
                             double gamma,
                             double costcut_threshold,
                             int32_t max_reuse_batch_num,
                             int32_t result_window_rotate_size)
      : keyword_str_(keyword_str) {
    inserting_stage_ = new inserting_stage(window_batch_size,
                                           batch_interval);
    inserted_stage_ = new proceeded_stage(1);
    before_calculate_filter_stage_ = new keyword_filter_stage(keyword_str);
    calculating_stage_ = new calculating_stage(scaling_param,
                                               gamma,
                                               costcut_threshold,
                                               max_reuse_batch_num);
    calculated_stage_ = new proceeded_stage(result_window_rotate_size);
    before_waiting_mix_filter_stage_ = new keyword_filter_stage(keyword_str);
    waiting_mix_stage_ = new waiting_mix_stage();
    batch_weight_reset_stage_ = new batch_weight_reset_stage();

    inserting_stage_->set_next(inserted_stage_);
    inserted_stage_->set_next(before_calculate_filter_stage_);
    before_calculate_filter_stage_->set_next(calculating_stage_);
    calculating_stage_->set_next(calculated_stage_);
    calculated_stage_->set_next(before_waiting_mix_filter_stage_);
    before_waiting_mix_filter_stage_->set_next(waiting_mix_stage_);
    batch_weight_reset_stage_->set_next(calculated_stage_);
  }

  burst_system::~burst_system() {
    delete inserting_stage_;
    delete inserted_stage_;
    delete before_calculate_filter_stage_;
    delete calculating_stage_;
    delete calculated_stage_;
    delete before_waiting_mix_filter_stage_;
    delete waiting_mix_stage_;
    delete batch_weight_reset_stage_;
  }

  bool burst_system::add_document(const std::string& doc_txt, double pos) {
    std::string::size_type index = doc_txt.find(keyword_str_);
    AddDocumentResults ret =
        index == std::string::npos ?
          inserting_stage_->add_document(1, 0, pos) :
          inserting_stage_->add_document(1, 1, pos);
    return ret != kPosIsOlderThanWindow;
  }

  window burst_system::get_result() const {
    return calculated_stage_->get_result();
  }

  window burst_system::get_result_at(double pos) const {
    return calculated_stage_->get_result_at(pos);
  }

  double burst_system::get_scaling_param() const {
    return calculating_stage_->get_scaling_param();
  }

  double burst_system::get_gamma() const {
    return calculating_stage_->get_gamma();
  }

  window burst_system::get_diff_impl() const {
    const window* w = waiting_mix_stage_->get_diff();
    if (w == 0) {
      return window();
    }
    const window ret = *w;
    delete_window_ptr(w);
    return ret;
  }

  void burst_system::put_diff_impl(const window& w) {
    DLOG(INFO) << "burst_system::put_diff_impl: "
               << "start pos of window: " << w.get_start_pos() << ", "
               << "batch_size: " << w.get_batch_size();

    window w1(w);
    window* w_ptr_to_calculated_stage = create_window_ptr(w1);
    calculated_stage_->push(w_ptr_to_calculated_stage);

    window* w_ptr_to_batch_weight_reset_stage = create_window_ptr(w);
    batch_weight_reset_stage_->push(w_ptr_to_batch_weight_reset_stage);
  }

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
