// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_ANOMALY_ANOMALY_STORAGE_BASE_HPP_
#define JUBATUS_CORE_ANOMALY_ANOMALY_STORAGE_BASE_HPP_

#include <iosfwd>
#include <string>
#include "../framework/mixable.hpp"

namespace jubatus {
namespace core {
namespace storage {

class anomaly_storage_base {
 public:
  virtual ~anomaly_storage_base() {
  }

  virtual void get_diff(std::string& diff) const = 0;
  virtual void set_mixed_and_clear_diff(const std::string& mixed_diff) = 0;
  virtual void mix(const std::string& lhs, std::string& rhs) const = 0;

  virtual void save(std::ostream& os) const = 0;
  virtual void load(std::istream& is) = 0;
};

struct mixable_anomaly_storage : framework::mixable<
    anomaly_storage_base,
    std::string> {
  std::string get_diff_impl() const {
    std::string diff;
    get_model()->get_diff(diff);
    return diff;
  }

  void put_diff_impl(const std::string& v) {
    get_model()->set_mixed_and_clear_diff(v);
  }

  void mix_impl(
      const std::string& lhs,
      const std::string& rhs,
      std::string& mixed) const {
    mixed = lhs;
    get_model()->mix(rhs, mixed);
  }

  void clear() {
  }
};

}  // namespace storage
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_ANOMALY_ANOMALY_STORAGE_BASE_HPP_
