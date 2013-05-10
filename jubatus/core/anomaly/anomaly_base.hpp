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

#include <pficommon/data/unordered_map.h>
#include <pficommon/lang/shared_ptr.h>

#include "../common/type.hpp"
#include "../storage/sparse_matrix_storage.hpp"
#include "anomaly_storage_base.hpp"
#include "anomaly_type.hpp"

namespace jubatus {
namespace core {
namespace anomaly {

class anomaly_base {
 public:
  anomaly_base();
  virtual ~anomaly_base();

  // return anomaly score of query
  virtual float calc_anomaly_score(const sfv_t& query) const = 0;
  virtual float calc_anomaly_score(const std::string& id) const = 0;
  virtual void clear() = 0;
  virtual void clear_row(const std::string& id) = 0;
  virtual void update_row(const std::string& id, const sfv_diff_t& diff) = 0;
  virtual void get_all_row_ids(std::vector<std::string>& ids) const = 0;

  virtual std::string type() const = 0;
  virtual core::storage::anomaly_storage_base* get_storage() = 0;
  virtual const core::storage::anomaly_storage_base*
      get_const_storage() const = 0;

  void save(std::ostream&);
  void load(std::istream&);

  // static float calc_distance(sfv_t& q1, sfv_t& q2);
  // static float calc_l2norm(sfv_t& query);

 protected:
  static const uint32_t NEIGHBOR_NUM;

  virtual bool save_impl(std::ostream&) = 0;
  virtual bool load_impl(std::istream&) = 0;

  core::storage::sparse_matrix_storage orig_;
};

}  // namespace anomaly
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_ANOMALY_ANOMALY_BASE_HPP_
