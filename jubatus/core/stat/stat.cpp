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
#include <string>
#include <utility>
#include "jubatus/util/system/time_util.h"
#include "jubatus/util/data/serialization.h"

using std::string;
using jubatus::util::system::time::clock_time;
using jubatus::util::system::time::get_clock_time;

namespace jubatus {
namespace core {
namespace stat {

stat::stat(size_t window_size)
    : window_size_(window_size),
      e_(0),
      n_(0) {
}

stat::~stat() {
}

void stat::get_diff(std::pair<double, size_t>& ret) const {
  ret.first = 0;
  ret.second = 0;

  for (jubatus::util::data::unordered_map<std::string, stat_val>::
      const_iterator p = stats_.begin(); p != stats_.end(); ++p) {
    double pr = p->second.n_;
    ret.first += pr * log(pr);
    ret.second += pr;
  }
}

void stat::set_mixed_and_clear_diff(const std::pair<double, size_t>& e) {
  e_ = e.first;
  n_ = e.second;
}

void stat::mix(
    const std::pair<double, size_t>& lhs,
    std::pair<double, size_t>& ret) const {
  ret.first += lhs.first;
  ret.second += lhs.second;
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
  jubatus::util::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.find(key);
  if (p == stats_.end()) {
    throw JUBATUS_EXCEPTION(stat_error("sum: key " + key + " not found"));
  }
  return p->second.sum_;
}

double stat::stddev(const std::string& key) const {
  jubatus::util::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.find(key);
  if (p == stats_.end()) {
    throw JUBATUS_EXCEPTION(stat_error("stddev: key " + key + " not found"));
  }
  const stat_val& st = p->second;
  return sqrt(moment(key, 2, st.sum_ / st.n_));
}

double stat::max(const std::string& key) const {
  jubatus::util::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.find(key);
  if (p == stats_.end()) {
    throw JUBATUS_EXCEPTION(stat_error("max: key " + key + " not found"));
  }
  const stat_val& st = p->second;
  return st.max_;
}

double stat::min(const std::string& key) const {
  jubatus::util::data::unordered_map<std::string, stat_val>::const_iterator p =
      stats_.find(key);
  if (p == stats_.end()) {
    throw JUBATUS_EXCEPTION(stat_error("min: key " + key + " not found"));
  }
  const stat_val& st = p->second;
  return st.min_;
}

double stat::entropy() const {
  if (n_ == 0) {
    // not MIXed ever yet
    size_t total = 0;
    for (jubatus::util::data::unordered_map<std::string, stat_val>::
        const_iterator p = stats_.begin(); p != stats_.end(); ++p) {
      total += p->second.n_;
    }
    double ret = 0;
    for (jubatus::util::data::unordered_map<std::string, stat_val>::
        const_iterator p = stats_.begin(); p != stats_.end(); ++p) {
      double pr = p->second.n_ / static_cast<double>(total);
      ret += pr * log(pr);
    }
    return -1.0 * ret;
  }
  double n = n_;
  return std::log(n) - e_ / n_;
}

double stat::moment(const std::string& key, int n, double c) const {
  if (n < 0) {
    return -1;
  }
  jubatus::util::data::unordered_map<std::string, stat_val>::const_iterator p =
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

void stat::pack(msgpack::packer<msgpack::sbuffer>& packer) const {
  packer.pack(*this);
}
void stat::unpack(msgpack::object o) {
  o.convert(this);
}
std::string stat::type() const {
  return "stat";
}

void stat::register_mixables_to_holder(framework::mixable_holder& holder)
    const {
  // TODO(beam2d): Split a part of MIX operations from |stat| to outside of it
  // and use it as a model. |shared_from_this| is a workaround to support this
  // canonical function like other algorithms.
  jubatus::util::lang::shared_ptr<mixable_stat> mixable(new mixable_stat);
  mixable->set_model(
      jubatus::util::lang::const_pointer_cast<stat>(shared_from_this()));
  holder.register_mixable(mixable);
}

}  // namespace stat
}  // namespace core
}  // namespace jubatus
