// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#pragma once

#include <algorithm>
#include <cmath>
#include "anomaly_base.hpp"
#include "../common/vector_util.hpp"

namespace jubatus {
namespace anomaly {

const uint64_t anomaly_base::neighbor

class anomaly_base{
public:
  anomaly_base();
  virtual ~anomaly_base();
  
  virtual void get_anomaly_score(const sfv_t& query, std::pair<std::string, float>& score, size_t neighbor_num) const = 0; //return anomaly score of query
  virtual void clear() = 0;
  virtual void clear_row(const std::string& id) = 0;
  virtual void update_row(const std::string& id, const sfv_diff_t& diff) = 0;
  virtual void get_all_row_ids(std::vector<std::string>& ids) const = 0;

  virtual std::string type() const = 0;
  virtual storage::anomaly_storage_base* get_storage() = 0;
  virtual const storage::anomaly_storage_base* get_const_storage() const = 0;

  
  void calc_anomaly_score(const std::string& id, std::pair<std::string, float>& score, size_t neighbor_num) const;

  void save(std::ostream&);
  void load(std::istUream&);

  static float calc_distance(sfv_t& q1, sfv_t& q2);
  static float calc_l2norm(sfv_t& q1, sfv_t& q2);


protected:
  virtual bool save_impl(std::ostream&) = 0;
  virtual bool load_impl(std::istream&) = 0;

  static const uint64_t complete_row_similar_num_;
  storage::sparse_matrix_storage orig_;
};

} // namespace anomaly
} // namespace jubatus
