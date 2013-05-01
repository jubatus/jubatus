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

#include "stat.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>
#include <string>
#include <pficommon/system/time_util.h>
#include <pficommon/data/serialization.h>

using std::string;
using pfi::system::time::clock_time;
using pfi::system::time::get_clock_time;

namespace jubatus {
namespace core {
namespace stat {

namespace {
inline double sq(double d) {
  return d * d;
}
}  // namespace

stat::stat(size_t window_size)
    : window_size_(window_size) {
}

stat::~stat() {
}

void stat::push(const std::string& key, double val) {
  {
    clock_time ct = get_clock_time();
    window_.push_back(make_pair((uint64_t) ct, make_pair(key, val)));
    stats_[key].add(val);
  }
  while (window_.size() > window_size_) {
    string key = window_.front().second.first;
    double val = window_.front().second.second;
    stat_val& st = stats_[key];
    window_.pop_front();
    st.rem(val, key, *this);
    if (st.n_ == 0) {
      stats_.erase(key);
    }
  }
}

double stat::sum(const std::string& key) const {
  pfi::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.find(key);
  if (p == stats_.end()) {
    throw JUBATUS_EXCEPTION(stat_error("sum: key " + key + " not found"));
  }
  return p->second.sum_;
}

double stat::stddev(const std::string& key) const {
  pfi::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.find(key);
  if (p == stats_.end()) {
    throw JUBATUS_EXCEPTION(stat_error("stddev: key " + key + " not found"));
  }
  const stat_val& st = p->second;
  return sqrt(moment(key, 2, st.sum_ / st.n_));
}

double stat::max(const std::string& key) const {
  pfi::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.find(key);
  if (p == stats_.end()) {
    throw JUBATUS_EXCEPTION(stat_error("max: key " + key + " not found"));
  }
  const stat_val& st = p->second;
  return st.max_;
}

double stat::min(const std::string& key) const {
  pfi::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.find(key);
  if (p == stats_.end()) {
    throw JUBATUS_EXCEPTION(stat_error("min: key " + key + " not found"));
  }
  const stat_val& st = p->second;
  return st.min_;
}

double stat::entropy() const {
  size_t total = 0;
  for (pfi::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.begin(); p != stats_.end(); ++p) {
    total += p->second.n_;
  }
  double ret = 0;
  for (pfi::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.begin(); p != stats_.end(); ++p) {
    double pr = p->second.n_ / static_cast<double>(total);
    ret += pr * log(pr);
  }
  return ret;
}

double stat::moment(const std::string& key, int n, double c) const {
  if (n < 0) {
    return -1;
  }
  pfi::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.find(key);
  if (p == stats_.end()) {
    throw JUBATUS_EXCEPTION(stat_error("min: key " + key + " not found"));
  }
  const stat_val& st = p->second;

  if (n == 0) {
    return 1;
  }

  if (n == 1) {
    return (st.sum_ - c * st.n_) / st.n_;
  }

  if (n == 2) {
    return (st.sum2_ - 2 * st.sum_ * c) / st.n_ + c * c;
  }

  // fallback
  double ret = 0;
  for (size_t i = 0; i < window_.size(); ++i) {
    if (window_[i].second.first != key) {
      continue;
    }
    ret += pow(window_[i].second.second - c, n);
  }
  return ret / st.n_;
}

void stat::clear() {
  window_.clear();
  stats_.clear();
}

bool stat::save(std::ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << *this;
  return true;
}
bool stat::load(std::istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> *this;
  return true;
}
std::string stat::type() const {
  return "stat";
}
}  // namespame stat
}  // namespace core
}  // namespace jubatus
