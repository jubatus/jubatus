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

#include <iosfwd>
#include <string>
#include <vector>
#include <pficommon/data/unordered_map.h>
#include <pficommon/data/unordered_set.h>
#include "../common/type.hpp"
#include "anomaly_storage_base.hpp"
#include "storage_type.hpp"


namespace jubatus {
namespace storage{

class lof_storage : public anomaly_storage_base {
public:
  lof_storage();
  lof_storage(const std::map<std::string, std::string>& config);
  virtual ~lof_storage();

  // same functions for recommender_storage
  void set_row(const std::string& row, const std::vector<float>& hash, float norm);
  void remove_row(const std::string& row);
  void clear();
  void get_all_row_ids(std::vector<std::string>& ids) const;

  // important functions for lof
  // for analyze
  void calc_lrd(const sfv_t& query, std::pair<std::string, float>& lrd_value, size_t neighbor_num) const;
  void similar_row_lrds(const sfv_t& query, std::map<std::string, float>& neighbor_lrd_values, size_t neighbor_num) const;
  // for update

  void similar_row(const std::vector<float>& hash,
                    float norm,
                   uint64_t probe_num,
                   uint64_t ret_num,
                   std::vector<std::pair<std::string, float> >& ids) const;
  std::string name() const;

  //size_t table_num() const { return table_num_; }
  //size_t all_lsh_num() const { return shift_.size(); }

  bool save(std::ostream& os);
  bool load(std::istream& is);

  virtual void get_diff(std::string& diff) const;
  virtual void set_mixed_and_clear_diff(const std::string& mixed_diff);
  virtual void mix(const std::string& lhs, std::string& rhs) const;

private:
  uint32_t neighbor_num_;
  float reverse_nn_coefficient_;

  // add anything you want
};

}
}
