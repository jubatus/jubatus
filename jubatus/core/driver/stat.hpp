// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_DRIVER_STAT_HPP_
#define JUBATUS_CORE_DRIVER_STAT_HPP_

#include <algorithm>
#include <string>
#include <utility>

#include "../stat/stat.hpp"
#include "../stat/mixable_stat.hpp"
#include "../framework/mixable.hpp"

namespace jubatus {
namespace core {
namespace driver {

struct mixable_stat : public framework::mixable<
    jubatus::core::stat::stat,
    std::pair<double, size_t> > {
 public:
  void clear() {
  }

  std::pair<double, size_t> get_diff_impl() const {
    return get_model()->get_diff();
  }

  void mix_impl(
      const std::pair<double, size_t>& lhs,
      const std::pair<double, size_t>& rhs,
      std::pair<double, size_t>& mixed) const {
    mixed = lhs;
    jubatus::core::stat::mixable_stat::reduce(rhs, mixed);
  }

  void put_diff_impl(const std::pair<double, size_t>& v) {
    get_model()->put_diff(v);
  }
};

class stat {
 public:
  explicit stat(jubatus::core::stat::stat* stat_method);
  virtual ~stat();

  pfi::lang::shared_ptr<framework::mixable_holder> get_mixable_holder() const {
    return mixable_holder_;
  }

  jubatus::core::stat::stat* get_model() const {
    return mixable_stat_model_.get_model().get();
  }

  void push(const std::string& key, double value);
  double sum(const std::string&) const;
  double stddev(const std::string&) const;
  double max(const std::string&) const;
  double min(const std::string&) const;
  double entropy() const;
  double moment(const std::string&, int, double) const;

 private:
  pfi::lang::shared_ptr<framework::mixable_holder> mixable_holder_;

  pfi::lang::shared_ptr<jubatus::core::stat::stat> stat_;
  mixable_stat mixable_stat_model_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_STAT_HPP_
