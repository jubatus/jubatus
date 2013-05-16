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

#include <cmath>
#include <string>
#include <utility>
#include "mixable_stat.hpp"

using std::pair;

namespace jubatus {
namespace core {
namespace stat {

mixable_stat::mixable_stat(size_t window_size)
    : stat(window_size),
      e_(0),
      n_(0) {
}

mixable_stat::~mixable_stat() {
}

std::pair<double, size_t> mixable_stat::get_diff() const {
  std::pair<double, size_t> ret;
  ret.first = 0;
  ret.second = 0;

  for (pfi::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.begin(); p != stats_.end(); ++p) {
    double pr = p->second.n_;
    ret.first += pr * log(pr);
    ret.second += pr;
  }
  return ret;
}

void mixable_stat::put_diff(const pair<double, size_t>& e) {
  e_ = e.first;
  n_ = e.second;
}

void mixable_stat::reduce(
    const pair<double, size_t>& lhs,
    pair<double, size_t>& ret) {
  ret.first += lhs.first;
  ret.second += lhs.second;
}

double mixable_stat::entropy() const {
  if (n_ == 0) {
    return 0;
  }
  double n = n_;
  return log(n) - e_ / n_;
}

bool mixable_stat::save(std::ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  stat::save(os);
  oa << *this;
  return true;
}
bool mixable_stat::load(std::istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  stat::load(is);
  ia >> *this;
  return true;
}

}  // namespace stat
}  // namespace core
}  // namespace jubatus
