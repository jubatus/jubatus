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

#ifndef JUBATUS_FV_CONVERTER_CIRCULAR_BUFFER_HPP_
#define JUBATUS_FV_CONVERTER_CIRCULAR_BUFFER_HPP_

#include <assert.h>

#include <stdint.h>
#include <vector>

namespace jubatus {
namespace fv_converter {

template<class T>
class circular_buffer {
 public:
  explicit circular_buffer(std::size_t size)
    : buf_size_(size), buf_(buf_size_), head_(0), tail_(0), empty_(true) {
  }

  void push(const T& x) {
    buf_[tail_] = x;

    if (!empty_ && head_ == tail_)
      move_indicator_to_next(head_);
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

  T& top() {
    assert(!empty_);
    return buf_[head_];
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

  std::vector<T> get_all() const {
    std::vector<T> ret;
    add_all(ret);
    return ret;
  }

  void add_all(std::vector<T>& ret) const {
    assert(ret.empty());
    if (!empty_) {
      indicator i = head_;
      ret.reserve(ret.size() + size());
      ret.push_back(buf_[i]);
      move_indicator_to_next(i);
      for (; i != tail_; move_indicator_to_next(i)) {
        ret.push_back(buf_[i]);
      }
    }
  }

 private:
  typedef int indicator;
  const std::size_t buf_size_;

  void move_indicator_to_next(indicator & i) const {
    ++i;
    i %= buf_size_;
  }

  std::vector<T>  buf_;
  indicator head_;
  indicator tail_;
  bool empty_;
};

}  // namespace fv_converter
}  // namespace jubatus

#endif  // JUBATUS_FV_CONVERTER_CIRCULAR_BUFFER_HPP_
