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

#ifndef JUBATUS_FV_CONVERTER_PROCESSING_WINDOW_HPP_
#define JUBATUS_FV_CONVERTER_PROCESSING_WINDOW_HPP_

#include <assert.h>
#include <stdint.h>

#include <string>
#include <utility>
#include <vector>

#include <pficommon/lang/cast.h>

#include "../common/type.hpp"
#include "windowset_config.hpp"
#include "num_rolling.hpp"

namespace jubatus {
namespace fv_converter {

template <typename T>
class processing_window {
 public:
  processing_window(
      std::size_t size,
      window::window_type type,
      const std::vector<window::window_process>& pr)
    : buf_size_(size),
      type_(type),
      buf_(buf_size_),
      head_(0),
      tail_(0),
      empty_(true) {
  }

  void push(uint64_t clock, const T& x) {
    if (!empty_ && head_ == tail_) {
      pop();
    }
    if (type_ == window::DATUM_UNIT) {
      while (!empty_ && buf_size_ < clock - buf_[head_].clock_) {
        pop();
      }
    }
    buf_[tail_] = window_entry<T>(clock, x);
    move_indicator_to_next(tail_);
    empty_ = false;
  }

  void pop() {
    if (!empty_) {
      move_indicator_to_next(head_);
      if (tail_ == head_) {
        empty_ = true;
      }
    } else {
      // do nothing as if std::stack.pop()
    }
  }

  const T& top() const {
    assert(!empty_);
    return buf_[head_].value_;
  }

  uint64_t top_clock() const {
    assert(!empty_);
    return buf_[head_].clock_;
  }

  void clear() {
    head_ = tail_ = 0;
    empty_ = true;
  }

  bool empty() const {
    return empty_;
  }

  std::size_t size() const {
    std::size_t sz = (buf_size_ + tail_ - head_) % buf_size_;
    return (sz == 0 && !empty_) ? buf_size_ : sz;
  }
  bool full() const {
    if (!empty_ && head_ == tail_) {
      return true;
    } else {
      return false;
    }
  }

  /*  // debug
  std::vector<double> get_all() const {
    std::vector<double> ret;
    add_all(ret);
    return ret;
  }
  //*/

  void add_all(
      std::vector<std::pair<uint64_t, T> >& ret,
      uint64_t clock) const {
    assert(ret.empty());
    if (!empty_) {
      ret.reserve(ret.size() + size());
      indicator i = head_;
      if (latest_clock() < clock && full()) {
        move_indicator_to_next(i);
      }

      if (type_ == window::DATUM_UNIT) {
        while (i != tail_ && buf_.size() < (clock - buf_[i].clock_)) {
          move_indicator_to_next(i);
        }
      }

      for (; i != tail_; move_indicator_to_next(i)) {
        ret.push_back(std::make_pair(buf_[i].clock_, buf_[i].value_));
      }
    } else {
      // std::cout << "empty add_all()!" << std::endl;
    }
  }

  void emit(
      const uint64_t clock,
      const std::string& key,
      const T* tmp_value,
      sfv_t& fv) const {
    // nothing todo
  }

 private:
  typedef int indicator;
  const std::size_t buf_size_;
  const window::window_type type_;
  const std::vector<window::window_process> process_;

  void move_indicator_to_next(indicator& i) const {
    ++i;
    i %= buf_size_;
  }
  void move_indicator_to_prev(indicator& i) const {
    --i;
    i = i < 0 ? buf_.size() - 1 : i;
  }

  num_window_entry::clock_type latest_clock() const {
    assert(!empty_);
    indicator i = tail_;
    move_indicator_to_prev(i);
    return buf_[i].clock_;
  }


  std::vector<window_entry<T> > buf_;
  std::vector<window::window_process> proccess_;
  indicator head_;
  indicator tail_;
  bool empty_;
};

template <>
class processing_window<double> {
 public:
  processing_window(
      std::size_t size,
      window::window_type type,
      const std::vector<window::window_process>& pr)
    : buf_size_(size),
      type_(type),
      buf_(buf_size_),
      head_(0),
      tail_(0),
      empty_(true) {
    for (size_t i = 0; i < pr.size(); ++i) {
      rolling_.push_back(
          pfi::lang::shared_ptr<num_rolling_base>
              (num_rolling_factory::create_rolling(size, pr[i])));
    }
  }

  void push(uint64_t clock, const double& x) {
    for (size_t i = 0; i < rolling_.size(); ++i) {
      rolling_[i]->push_hook(num_window_entry(clock, x));
    }

    if (!empty_ && head_ == tail_) {
      pop();
    }
    if (type_ == window::DATUM_UNIT) {
      while (!empty_ && buf_size_ < clock - buf_[head_].clock_) {
        pop();
      }
    }
    buf_[tail_] = num_window_entry(clock, x);
    move_indicator_to_next(tail_);
    empty_ = false;
  }

  void pop() {
    if (!empty_) {
      for (size_t i = 0; i < rolling_.size(); ++i) {
        rolling_[i]->pop_hook(
            num_window_entry(buf_[head_].clock_, buf_[head_].value_));
      }
      move_indicator_to_next(head_);
      if (tail_ == head_) {
        empty_ = true;
      }
    } else {
      // do nothing as if std::stack.pop()
    }
  }

  double top() const {
    assert(!empty_);
    return buf_[head_].value_;
  }

  uint64_t top_clock() const {
    assert(!empty_);
    return buf_[head_].clock_;
  }

  void clear() {
    head_ = tail_ = 0;
    empty_ = true;
  }

  bool empty() const {
    return empty_;
  }

  std::size_t size() const {
    std::size_t sz = (buf_size_ + tail_ - head_) % buf_size_;
    return (sz == 0 && !empty_) ? buf_size_ : sz;
  }

  bool full() const {
    indicator next_tail = tail_;
    move_indicator_to_next(next_tail);

    if (head_ == next_tail) {
      return true;
    } else {
      return false;
    }
  }
  /*  // debug
  std::vector<double> get_all() const {
    std::vector<double> ret;
    add_all(ret);
    return ret;
  }
  //*/

  void add_all(
      std::vector<std::pair<uint64_t, double> >& ret,
      uint64_t clock) const {
    assert(ret.empty());
    if (!empty_) {
      ret.reserve(ret.size() + size());
      indicator i = head_;
      if (latest_clock() < clock && full()) {
        move_indicator_to_next(i);
      }

      if (type_ == window::DATUM_UNIT) {
        while (i != tail_ && buf_.size() < (clock - buf_[i].clock_)) {
          move_indicator_to_next(i);
        }
      }
      for (; i != tail_; move_indicator_to_next(i)) {
        ret.push_back(std::make_pair(buf_[i].clock_, buf_[i].value_));
      }
    }
  }

  void emit(
      const uint64_t clock,
      const std::string& key,
      const double* tmp_value,
      sfv_t& fv) const {
    if (empty()) {
      return;
    }
    std::vector<num_window_entry> hide_entries;
    hide_entries.reserve(buf_.size());
    indicator i = head_;
    move_indicator_to_next(i);

    if (latest_clock() < clock && full()) {
      // if buffer is full, temporary poping tail must happen
      hide_entries.push_back(buf_[head_]);
    }

    if (type_ == window::DATUM_UNIT) {
      while (i != tail_ && buf_.size() < (clock - buf_[i].clock_)) {
        hide_entries.push_back(buf_[i]);
        move_indicator_to_next(i);
      }
    }

    for (size_t j = 0; j < rolling_.size(); ++j) {
      if (tmp_value != NULL) {
        rolling_[j]->emit(
            key + "_" + pfi::lang::lexical_cast<std::string>(buf_size_),
            fv,
            num_window_entry(clock, *tmp_value),
            hide_entries);
      } else {
        rolling_[j]->emit(
          key + "_" + pfi::lang::lexical_cast<std::string>(buf_size_),
          fv,
          hide_entries);
      }
    }
  }

 private:
  typedef int indicator;
  const std::size_t buf_size_;
  const window::window_type type_;
  const std::vector<window::window_process> process_;

  void move_indicator_to_next(indicator& i) const {
    i = (i + 1) % buf_size_;
  }
  void move_indicator_to_prev(indicator& i) const {
    --i;
    i = i < 0 ? buf_.size() - 1 : i;
  }

  num_window_entry::clock_type latest_clock() const {
    assert(!empty_);
    indicator i = tail_;
    move_indicator_to_prev(i);
    return buf_[i].clock_;
  }

  std::vector<num_window_entry> buf_;
  std::vector<window::window_process> proccess_;
  std::vector<pfi::lang::shared_ptr<num_rolling_base> > rolling_;
  indicator head_;
  indicator tail_;
  bool empty_;
};

}  // namespace fv_converter
}  // namespace jubatus

#endif  // JUBATUS_FV_CONVERTER_CIRCULAR_BUFFER_HPP_
