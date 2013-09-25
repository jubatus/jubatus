// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_FV_CONVERTER_NUM_ROLLING_IMPL_HPP_
#define JUBATUS_FV_CONVERTER_NUM_ROLLING_IMPL_HPP_

#include <algorithm>
#include <deque>
#include <string>
#include <utility>
#include <vector>

#include <pficommon/text/json.h>
#include <pficommon/lang/cast.h>
#include "../common/jsonconfig.hpp"
#include "../common/exception.hpp"
#include "num_rolling.hpp"
#include "windowset_config.hpp"

namespace jubatus {
namespace fv_converter {

class num_rolling_max : public num_rolling_base {
 public:
  virtual ~num_rolling_max() {}
  void push_hook(const num_window_entry& new_entry) {
    while (!max_list_.empty() && max_list_.back().value_ < new_entry.value_) {
      max_list_.pop_back();
    }
    max_list_.push_back(new_entry);
  }
  void pop_hook(const num_window_entry& victim_entry) {
    while (!max_list_.empty() &&
           max_list_.front().clock_ < victim_entry.clock_) {
      max_list_.pop_front();
    }
  }

  void emit(
      const std::string& key,
      sfv_t& output,
      const std::vector<num_window_entry>& hide_entries) const {
    const size_t max_index = offset_maximum(hide_entries);
    if (max_list_.size() <= max_index) {
      return;
    }
    output.push_back(
        std::make_pair(
            key + "_max_", max_list_[max_index].value_));
  }
  void emit(
      const std::string& key,
      sfv_t& output,
      const num_window_entry& see_entry,
      const std::vector<num_window_entry>& hide_entries) const {
    const size_t max_index = offset_maximum(hide_entries);
    if (max_list_.size() <= max_index) {
      output.push_back(
        std::make_pair(
            key + "_max_",
            see_entry.value_));
    } else {
      output.push_back(
          std::make_pair(
              key + "_max_",
              std::max(max_list_[max_index].value_, see_entry.value_)));
    }
  }

  window::window_process proc_type() const {
    return window::MAX;
  }

 private:
  size_t offset_maximum(
      const std::vector<num_window_entry>& hide_entries) const {
    if (hide_entries.empty()) {
      return 0;
    }
    size_t index = 0;
    for (; max_list_.size() < index &&
             max_list_[index].clock_ <= hide_entries.back().clock_;
         ++index);
    return index;
  }
  std::deque<num_window_entry> max_list_;
};

class num_rolling_min : public num_rolling_base {
 public:
  virtual ~num_rolling_min() {}
  void push_hook(const num_window_entry& new_entry) {
    while (!min_list_.empty() && new_entry.value_ < min_list_.back().value_) {
      min_list_.pop_back();
    }
    min_list_.push_back(new_entry);
  }
  void pop_hook(const num_window_entry& victim_entry) {
    while (!min_list_.empty() &&
           min_list_.front().clock_ < victim_entry.clock_) {
      min_list_.pop_front();
    }
  }
  void emit(
      const std::string& key,
      sfv_t& output,
      const std::vector<num_window_entry>& hide_entries) const {
    const size_t min_index = offset_minimum(hide_entries);
    if (min_list_.size() <= min_index) {
      return;
    }
    output.push_back(
        std::make_pair(
            key + "_min_", min_list_[min_index].value_));
  }

  void emit(
      const std::string& key,
      sfv_t& output,
      const num_window_entry& see_entry,
      const std::vector<num_window_entry>& hide_entries) const {
    const size_t min_index = offset_minimum(hide_entries);
    if (min_list_.size() <= min_index) {
      output.push_back(
        std::make_pair(
            key + "_min_",
            see_entry.value_));
    } else {
      output.push_back(
          std::make_pair(
              key + "_min_",
              std::min(min_list_[min_index].value_, see_entry.value_)));
    }
  }
  window::window_process proc_type() const {
    return window::MIN;
  }

 private:
  size_t offset_minimum(
      const std::vector<num_window_entry>& hide_entries) const {
    if (hide_entries.empty()) {
      return 0;
    }
    size_t index = 0;
    for (; min_list_.size() < index &&
             min_list_[index].clock_ <= hide_entries.back().clock_;
         ++index);
    return index;
  }
  std::deque<num_window_entry> min_list_;
};

class num_rolling_sum : public num_rolling_base {
 public:
  num_rolling_sum() : sum_(0), min_clock_(0) {}
  virtual ~num_rolling_sum() {}
  void push_hook(const num_window_entry& new_entry) {
    sum_ += new_entry.value_;
  }
  void pop_hook(const num_window_entry& victim_entry) {
    sum_ -= victim_entry.value_;
    min_clock_ = min_clock_ < victim_entry.clock_ ?
                              victim_entry.clock_ : min_clock_;
  }
  void emit(
      const std::string& key,
      sfv_t& output,
      const std::vector<num_window_entry>& hide_entries) const {
    output.push_back(
        std::make_pair(key + "_sum_", sum_ - calc_accum(hide_entries)));
  }
  void emit(
      const std::string& key,
      sfv_t& output,
      const num_window_entry& see_entry,
      const std::vector<num_window_entry>& hide_entries) const {
    output.push_back(
        std::make_pair(
            key + "_sum_",
            sum_ + see_entry.value_ - calc_accum(hide_entries)));
  }
  window::window_process proc_type() const {
    return window::SUM;
  }

 protected:
  static double calc_accum(const std::vector<num_window_entry>& entries) {
    double accum = 0;
    for (size_t i = 0; i < entries.size(); ++i) {
      accum += entries[i].value_;
    }
    return accum;
  }
  double sum_;
  uint64_t min_clock_;
};

class num_rolling_avg : public num_rolling_sum {
 public:
  num_rolling_avg(): datums_(0) {}
  virtual ~num_rolling_avg() {}
  void push_hook(const num_window_entry& new_entry) {
    num_rolling_sum::push_hook(new_entry);
    ++datums_;
  }
  void pop_hook(const num_window_entry& victim_entry) {
    num_rolling_sum::pop_hook(victim_entry);
    --datums_;
  }
  void emit(
      const std::string& key,
      sfv_t& output,
      const std::vector<num_window_entry>& hide_entries) const {
    if (datums_ <= hide_entries.size()) {
      return;
    }
    output.push_back(
      std::make_pair(
          key + "_avg_",
          (sum_ - calc_accum(hide_entries)) / (datums_ - hide_entries.size())));
  }
  void emit(
      const std::string& key,
      sfv_t& output,
      const num_window_entry& see_entry,
     const std::vector<num_window_entry>& hide_entries) const {
    if (datums_ < hide_entries.size()) {
      return;
    }
    output.push_back(
        std::make_pair(
            key + "_avg_",
            (sum_ + see_entry.value_ - calc_accum(hide_entries)) /
            (datums_ + 1 - hide_entries.size())));
  }
  window::window_process proc_type() const {
    return window::AVG;
  }

 protected:
  uint64_t datums_;
};

class num_rolling_var : public num_rolling_avg {
 public:
  num_rolling_var() : squared_sum_(0) {}
  virtual ~num_rolling_var() {}
  void push_hook(const num_window_entry& new_entry) {
    num_rolling_avg::push_hook(new_entry);
    squared_sum_ += new_entry.value_ * new_entry.value_;
  }
  void pop_hook(const num_window_entry& victim_entry) {
    num_rolling_avg::pop_hook(victim_entry);
    squared_sum_ -= victim_entry.value_ * victim_entry.value_;
  }
  void emit(
      const std::string& key,
      sfv_t& output,
      const std::vector<num_window_entry>& hide_entries) const {
    double hide_accum_sum = 0;
    double hide_accum_squ = 0;
    for (size_t i = 0; i < hide_entries.size(); ++i) {
      hide_accum_sum += hide_entries[i].value_;
      hide_accum_squ += hide_entries[i].value_ * hide_entries[i].value_;
    }

    const double avg =
        (sum_ - hide_accum_sum) / (datums_ - hide_entries.size());
    output.push_back(
      std::make_pair(
          key + "_var_",
          ((squared_sum_ - hide_accum_squ) / (datums_ - hide_entries.size())) -
            (avg * avg)));
  }
  void emit(
      const std::string& key,
      sfv_t& output,
      const num_window_entry& see_entry,
      const std::vector<num_window_entry>& hide_entries) const {
    double hide_accum_sum = 0;
    double hide_accum_squ = 0;
    for (size_t i = 0; i < hide_entries.size(); ++i) {
      hide_accum_sum += hide_entries[i].value_;
      hide_accum_squ += hide_entries[i].value_ * hide_entries[i].value_;
    }
    const double avg =
        (sum_ + see_entry.value_ - hide_accum_sum) /
        (datums_ + 1 - hide_entries.size());
    output.push_back(
        std::make_pair(
            key + "_var_",
            ((squared_sum_ +
                (see_entry.value_ * see_entry.value_) -
                hide_accum_squ) /
            (datums_ + 1 - hide_entries.size())) -
            (avg * avg)));
  }

  window::window_process proc_type() const {
    return window::VAR;
  }

 protected:
  double squared_sum_;
};

}  // namespace fv_converter
}  // jubatus


#endif  // JUBATUS_FV_CONVERTER_NUM_ROLLING_IMPL_HPP_
