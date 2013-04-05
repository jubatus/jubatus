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

#ifndef JUBATUS_NEAREST_NEIGHBOR_BIT_VECTOR_NEAREST_NEIGHBOR_BASE_HPP_
#define JUBATUS_NEAREST_NEIGHBOR_BIT_VECTOR_NEAREST_NEIGHBOR_BASE_HPP_

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "nearest_neighbor_base.hpp"

namespace jubatus {
namespace nearest_neighbor {

class bit_vector_nearest_neighbor_base : public nearest_neighbor_base {
 public:
  bit_vector_nearest_neighbor_base(uint32_t bitnum,
                                   table::column_table* table,
                                   const std::string& id);
  bit_vector_nearest_neighbor_base(uint32_t bitnum,
                                   table::column_table* table,
                                   std::vector<table::column_type>& schema,
                                   const std::string& id);

  uint32_t bitnum() const { return bitnum_; }

  virtual void set_row(const std::string& id, const sfv_t& sfv);
  virtual void neighbor_row(const sfv_t& query,
                            std::vector<std::pair<std::string, float> >& ids,
                            uint64_t ret_num) const;
  virtual void neighbor_row(const std::string& query_id,
                            std::vector<std::pair<std::string, float> >& ids,
                            uint64_t ret_num) const;

 protected:
  virtual table::bit_vector hash(const sfv_t& sfv) const = 0;

 private:
  void fill_schema(std::vector<table::column_type>& schema);
  table::const_bit_vector_column bit_vector_column() const;

  void neighbor_row_from_hash(const table::bit_vector& query,
                              std::vector<std::pair<std::string, float> >& ids,
                              uint64_t ret_num) const;

  uint64_t bit_vector_column_id_;
  uint32_t bitnum_;
};

}  // namespace nearest_neighbor
}  // namespace jubatus

#endif  // JUBATUS_NEAREST_NEIGHBOR_BIT_VECTOR_NEAREST_NEIGHBOR_BASE_HPP_
