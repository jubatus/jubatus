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

#ifndef JUBATUS_CORE_ANOMALY_ANOMALY_BASE_HPP_
#define JUBATUS_CORE_ANOMALY_ANOMALY_BASE_HPP_

#include <string>
#include <vector>

#include "jubatus/util/data/unordered_map.h"
#include "jubatus/util/lang/shared_ptr.h"

#include "../common/type.hpp"
#include "../framework/mixable.hpp"
#include "../storage/sparse_matrix_storage.hpp"
#include "anomaly_type.hpp"

namespace jubatus {
namespace core {
namespace anomaly {

class anomaly_base {
 public:
  anomaly_base();
  virtual ~anomaly_base();

  // Calculates and returns anomaly score of given query.
  virtual float calc_anomaly_score(const common::sfv_t& query) const = 0;

  // Returns anomaly score of the row corresponding to given id.
  virtual float calc_anomaly_score(const std::string& id) const = 0;

  // Clears all rows.
  virtual void clear() = 0;

  // Removes the row corresponding to given id.
  //
  // The removal event must be shared among other MIX participants. Thus,
  // typical implementation does not eliminate the row immediately but marks it
  // as "removed" instead. Some implementations including light_lof do not
  // support this function.
  virtual void clear_row(const std::string& id) = 0;

  // Partially updates the row corresponding to given id.
  //
  // Some implementations including light_lof do not support this function.
  virtual void update_row(const std::string& id, const sfv_diff_t& diff) = 0;

  // Updates the row corresponding to given id.
  //
  // Some implementations including lof do not support this function.
  virtual void set_row(const std::string& id, const common::sfv_t& sfv) = 0;

  virtual void get_all_row_ids(std::vector<std::string>& ids) const = 0;
  virtual std::string type() const = 0;
  virtual void register_mixables_to_holder(
      framework::mixable_holder& holder) const = 0;

  uint64_t find_max_int_id() const;
 protected:
  // static const uint32_t NEIGHBOR_NUM;
};

}  // namespace anomaly
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_ANOMALY_ANOMALY_BASE_HPP_
