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

#ifndef JUBATUS_CORE_DRIVER_ANOMALY_HPP_
#define JUBATUS_CORE_DRIVER_ANOMALY_HPP_

#include <string>
#include <utility>
#include <vector>

#include "jubatus/util/lang/shared_ptr.h"
#include "../anomaly/anomaly_base.hpp"
#include "../framework/mixable.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"

namespace jubatus {
namespace core {
namespace driver {

class anomaly {
 public:
  anomaly(
      jubatus::util::lang::shared_ptr<core::anomaly::anomaly_base>
          anomaly_method,
      jubatus::util::lang::shared_ptr<fv_converter::datum_to_fv_converter>
          converter);
  virtual ~anomaly();

  jubatus::util::lang::shared_ptr<framework::mixable_holder>
  get_mixable_holder() const {
    return mixable_holder_;
  }

  jubatus::core::anomaly::anomaly_base* get_model() const {
    return anomaly_.get();
  }

  void clear_row(const std::string& id);
  std::pair<std::string, float> add(
      const std::string& id,
      const fv_converter::datum& d);
  float update(const std::string& id, const fv_converter::datum& d);
  float overwrite(const std::string& id, const fv_converter::datum& d);
  void clear();
  float calc_score(const fv_converter::datum& d) const;
  std::vector<std::string> get_all_rows() const;
  uint64_t find_max_int_id() const;

 private:
  jubatus::util::lang::shared_ptr<framework::mixable_holder> mixable_holder_;

  jubatus::util::lang::shared_ptr<fv_converter::datum_to_fv_converter>
      converter_;
  jubatus::util::lang::shared_ptr<core::anomaly::anomaly_base> anomaly_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_ANOMALY_HPP_
