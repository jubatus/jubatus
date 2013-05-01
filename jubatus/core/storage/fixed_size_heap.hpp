// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_STORAGE_FIXED_SIZE_HEAP_HPP_
#define JUBATUS_CORE_STORAGE_FIXED_SIZE_HEAP_HPP_

#include <algorithm>
#include <functional>
#include <vector>

namespace jubatus {
namespace core {
namespace storage {

template <typename T, typename Comp = std::less<T> >
class fixed_size_heap {
 public:
  explicit fixed_size_heap(size_t max)
      : max_size_(max),
        comp_(Comp()) {
  }

  fixed_size_heap(size_t max, const Comp& comp)
      : max_size_(max),
        comp_(comp) {
  }

  void push(const T& v) {
    if (data_.size() < max_size_) {
      data_.push_back(v);
      if (data_.size() == max_size_) {
        std::make_heap(data_.begin(), data_.end(), comp_);
      }
    } else {
      if (comp_(v, data_.front())) {
        std::pop_heap(data_.begin(), data_.end(), comp_);
        data_.back() = v;
        std::push_heap(data_.begin(), data_.end(), comp_);
      }
    }
  }

  void get_sorted(std::vector<T>& v) const {
    std::vector<T> vec(data_);
    std::sort(vec.begin(), vec.end(), comp_);
    v.swap(vec);
  }

  size_t size() const {
    return data_.size();
  }

  size_t get_max_size() const {
    return max_size_;
  }

 private:
  std::vector<T> data_;
  const size_t max_size_;
  const Comp comp_;
};

}  // namespace storage
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_STORAGE_FIXED_SIZE_HEAP_HPP_
