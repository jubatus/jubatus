// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "discrete_distribution.hpp"

#include <vector>

using std::vector;

namespace jubatus {
namespace core {
namespace clustering {

discrete_distribution::discrete_distribution(
    vector<double>::iterator begin,
    vector<double>::iterator end)
    : r_(),
      whist_() {
  sum_ = 0;
  for (vector<double>::iterator it = begin; it != end; ++it) {
    sum_ += *it;
    whist_.push_back(sum_);
  }
}

size_t discrete_distribution::operator()() {
  double d = r_.next_double() * sum_;
  return convert_(d);
}

size_t discrete_distribution::convert_(double d) {
  size_t low = 0;
  size_t high = whist_.size() - 1;
  size_t mid = 0;
  while (low < high) {
    mid = (low + high) / 2;
    if (whist_[mid] < d) {
      if (low == mid) {
        break;
      }
      low = mid;
    } else {
      if (high == mid) {
        break;
      }
      high = mid;
    }
  }
  return high;
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
