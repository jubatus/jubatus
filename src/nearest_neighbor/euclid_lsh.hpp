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

#ifndef JUBATUS_NEAREST_NEIGHBOR_EUCLID_LSH_HPP_
#define JUBATUS_NEAREST_NEIGHBOR_EUCLID_LSH_HPP_

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "nearest_neighbor_base.hpp"

namespace jubatus {
namespace nearest_neighbor {

class euclid_lsh : public nearest_neighbor_base {
 public:
  euclid_lsh(const std::map<std::string, std::string>& config,
             table::column_table* table, const std::string& id);
  euclid_lsh(const std::map<std::string, std::string>& config,
             table::column_table* table,
             std::vector<table::column_type>& schema, const std::string& id);

  virtual std::string type() const { return "euclid_lsh"; }

  virtual void set_row(const std::string& id, const sfv_t& sfv);
  virtual void neighbor_row(const sfv_t& query,
                            std::vector<std::pair<std::string, float> >& ids,
                            uint64_t ret_num) const;
  virtual void neighbor_row(const std::string& query,
                            std::vector<std::pair<std::string, float> >& ids,
                            uint64_t ret_num) const;

  virtual float calc_similarity(float distance) const {
    return -distance;
  }

 private:
  void set_config(const std::map<std::string, std::string>& config);
  void fill_schema(std::vector<table::column_type>& schema);
  table::const_bit_vector_column lsh_column() const;
  table::const_float_column norm_column() const;

  void neighbor_row_from_hash(const table::bit_vector& bv,
                              float norm,
                              std::vector<std::pair<std::string, float> >& ids,
                              uint64_t ret_num) const;

  uint64_t first_column_id_;
  uint32_t hash_num_;
};

}  // namespace nearest_neighbor_base
}  // namespace jubatus

#endif  // JUBATUS_NEAREST_NEIGHBOR_EUCLID_LSH_HPP_
