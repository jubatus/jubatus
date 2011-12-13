#pragma once

#include <algorithm>

namespace jubatus {
namespace storage {

template <typename T, typename Comp = std::less<T> >
class fixed_size_heap {
 public:
  fixed_size_heap(size_t max)
      : max_size_(max), comp_(Comp()) {}

  fixed_size_heap(size_t max, const Comp& comp)
      : max_size_(max), comp_(comp) {}

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

}
}
