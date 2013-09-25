// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_FV_CONVERTER_WINDOW_HPP_
#define JUBATUS_FV_CONVERTER_WINDOW_HPP_

#include <stdint.h>
#include <deque>
#include <vector>
#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/unordered_map.h>
#include "../common/unordered_map.hpp"

namespace jubatus {
namespace fv_converter {

namespace detail {
template <typename T>
class window {
 public:
  window(size_t length) : length_(length) {
  }
  void append(const T& new_value) {
    window_.push_front(new_value);
    while (length_ < window_.size()) {
      window_.pop_back();
    }
  }
  void windowize(std::vector<T>& target) const {
    target.reserve(target.size() + window_.size());
    for (typename buffer_type::const_iterator it = window_.begin();
         it != window_.end();
         ++it) {
      target.push_back(*it);
    }
  }
 private:
  typedef std::deque<T> buffer_type;
  size_t length_;
  buffer_type window_;
};

}  // namespace detail
}  // namespace fv_converter
}  // namespace jubatus

#endif  // JUBATUS_FV_CONVERTER_WINDOW_HPP_
