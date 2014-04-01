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

#include <limits.h>
#include <math.h>

#include <iostream>
#include <numeric>
#include <vector>

#include "window.hpp"

#define COPY_MEMBER(member)                                             \
  bool window::ccopy_##member(const window& w,                          \
                              const int32_t max_reuse_batch_num) {      \
    if (get_start_pos() < w.get_start_pos()) {                          \
      return false;                                                     \
    }                                                                   \
    const int batches_gap                                               \
      = (get_start_pos() - w.get_start_pos()) / w.get_batch_interval(); \
    const int batches_intersection = w.get_batch_size() - batches_gap;  \
    if (batches_intersection <= 0) {                                    \
      return true;                                                      \
    }                                                                   \
    const int copying_batch_size = w.get_batch_size();                  \
    const int reuse_batch_num                                           \
      = (max_reuse_batch_num < batches_intersection)                    \
      ? max_reuse_batch_num : batches_intersection;                     \
    for (int batch_id = 0; batch_id < reuse_batch_num; batch_id++) {    \
      batch b(w.get_batches().at(copying_batch_size - batches_intersection \
                                 + batch_id));                          \
      batches_.at(batch_id).                                            \
        copy_##member(w.get_batches().at(copying_batch_size             \
                                         - batches_intersection         \
                                         + batch_id));                  \
    }                                                                   \
    return true;                                                        \
  }


namespace jubatus {
namespace core {
namespace burst {
namespace system {

window::window() {
}

window::window(const double& pos, const int32_t window_batch_size,
               const int32_t batch_interval)
  : batch_interval_(batch_interval) {
  const int pos_batch_id_from_origin = floor(pos / batch_interval);
  const int window_start_batch_id_from_origin
    = pos_batch_id_from_origin - window_batch_size + 1;
  start_pos_ = window_start_batch_id_from_origin * batch_interval;
  batch b;
  batches_.assign(window_batch_size, b);
}

window::window(const window& w)
  : batches_(w.get_batches()),
    start_pos_(w.get_start_pos()),
    batch_interval_(w.get_batch_interval()) {
}

AddDocumentResults window::add_document(const uint32_t d,
                                        const uint32_t r,
                                        const double& pos) {
  if (pos < start_pos_) {
    return kPosIsOlderThanWindow;
  }
  const int batch_id = get_batch_id(pos);
  if (batch_id < 0) {
    return kPosIsNewerThanWindow;
  }
  batches_.at(batch_id).add_document(d, r);
  return kPosIsInWindow;
}

bool window::copy_d_r(const window& w) {
  return window::ccopy_d_r(w, INT_MAX);
}

bool window::copy_batch_weight(const window& w,
                                     const int32_t max_reuse_batch_num) {
  return window::ccopy_batch_weight(w, max_reuse_batch_num);
}

COPY_MEMBER(d_r)

COPY_MEMBER(batch_weight)

int32_t window::get_end_pos() const {
  return start_pos_ + batch_interval_ * get_batch_size();
}

int32_t window::get_batch_size() const {
  return batches_.size();
}

std::vector<uint32_t> window::get_d_vec() const {
  std::vector<uint32_t> ret;
  const int batch_size = batches_.size();
  for (int batch_id = 0; batch_id < batch_size; batch_id++) {
    ret.push_back(batches_.at(batch_id).get_d());
  }
  return ret;
}

std::vector<uint32_t> window::get_r_vec() const {
  std::vector<uint32_t> ret;
  const int batch_size = batches_.size();
  for (int batch_id = 0; batch_id < batch_size; batch_id++) {
    ret.push_back(batches_.at(batch_id).get_r());
  }
  return ret;
}

std::vector<double> window::get_batch_weights() const {
  std::vector<double> ret;
  const int batch_size = batches_.size();
  for (int batch_id = 0; batch_id < batch_size; batch_id++) {
    ret.push_back(batches_.at(batch_id).get_batch_weight());
  }
  return ret;
}

void window::set_batch_weights(const std::vector<double>& batch_weights) {
  const int batch_size = batches_.size();
  for (int batch_id = 0; batch_id < batch_size; batch_id++) {
    batches_.at(batch_id).set_batch_weight(batch_weights.at(batch_id));
  }
}

bool window::is_pos_older_than_me(const double& pos) const {
  if (pos < start_pos_) {
    return true;
  }
  return false;
}

bool window::is_pos_newer_than_me(const double& pos) const {
  if (get_end_pos() <= pos) {
    return true;
  }
  return false;
}

bool window::is_pos_in_me(const double& pos) const {
  if (is_pos_older_than_me(pos)) {
    return false;
  } else if (is_pos_newer_than_me(pos)) {
    return false;
  }
  return true;
}

bool window::is_bursted_at_latest_batch() const {
  if (!is_valid()) {
    return false;
  }
  batch latest_batch = batches_.back();
  if (0 < latest_batch.get_batch_weight()) {
    return true;
  }
  return false;
}

bool window::is_bursted_at(const double& pos) const {
  if (!is_valid()) {
    return false;
  }
  const int batch_id = get_batch_id(pos);
  batch pos_batch = batches_.at(batch_id);
  if (0 < pos_batch.get_batch_weight()) {
    return true;
  }
  return true;
}

bool window::is_valid() const {
  if (batches_.size() == 0) {
    return false;
  }
  return true;
}

bool window::is_more_valid_in_mix_than(const window& w) const {
  if (am_i_newer_than(w)) {
    return true;
  }
  std::vector<uint32_t> d_vec_of_mine = get_d_vec();
  std::vector<uint32_t> d_vec_of_target = w.get_d_vec();
  uint32_t sum_of_d_vec_of_mine = std::accumulate(d_vec_of_mine.begin(),
                                                  d_vec_of_mine.end(),
                                                  0);
  uint32_t sum_of_d_vec_of_target = std::accumulate(d_vec_of_target.begin(),
                                                    d_vec_of_target.end(),
                                                    0);

  std::vector<uint32_t> r_vec_of_mine = get_r_vec();
  std::vector<uint32_t> r_vec_of_target = w.get_r_vec();
  uint32_t sum_of_r_vec_of_mine = std::accumulate(r_vec_of_mine.begin(),
                                                  r_vec_of_mine.end(),
                                                  0);
  uint32_t sum_of_r_vec_of_target = std::accumulate(d_vec_of_target.begin(),
                                                    d_vec_of_target.end(),
                                                    0);
  return (sum_of_d_vec_of_mine + sum_of_r_vec_of_mine)
    > (sum_of_d_vec_of_target + sum_of_r_vec_of_target);
}

int window::get_batch_id(const double& pos) const {
  const int batch_size = batches_.size();
  int batch_start_pos = start_pos_;
  int batch_end_pos = start_pos_ + batch_interval_;
  for (int batch_id = 0; batch_id < batch_size; batch_id++) {
    if ((batch_start_pos <= pos) && (pos < batch_end_pos)) {
      return batch_id;
    }
    batch_start_pos += batch_interval_;
    batch_end_pos += batch_interval_;
  }
  return -1;
}

bool window::am_i_newer_than(const window& w) const {
  if (w.get_start_pos() < start_pos_) {
    return true;
  }
  return false;
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
