// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_STORAGE_BIT_INDEX_STORAGE_HPP_
#define JUBATUS_CORE_STORAGE_BIT_INDEX_STORAGE_HPP_

#include <string>
#include <utility>
#include <vector>
#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/unordered_map.h>
#include "../common/key_manager.hpp"
#include "storage_type.hpp"
#include "sparse_matrix_storage.hpp"
#include "bit_vector.hpp"
#include "recommender_storage_base.hpp"

namespace jubatus {
namespace core {
namespace storage {

class bit_index_storage : public recommender_storage_base {
 public:
  bit_index_storage();
  ~bit_index_storage();

  void set_row(const std::string& row, const bit_vector& bv);
  void get_row(const std::string& row, bit_vector& bv) const;
  void remove_row(const std::string& row);
  void clear();
  void get_all_row_ids(std::vector<std::string>& ids) const;

  void similar_row(
      const bit_vector& bv,
      std::vector<std::pair<std::string, float> >& ids,
      uint64_t ret_num) const;
  std::string name() const;

  bool save(std::ostream& os);
  bool load(std::istream& is);

  void get_diff(std::string& diff) const;
  void set_mixed_and_clear_diff(const std::string& mixed_diff);
  void mix(const std::string& lhs, std::string& rhs) const;

 private:
  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(bitvals_) & MEMBER(bitvals_diff_);
  }

  bit_table_t bitvals_;
  bit_table_t bitvals_diff_;
};

}  // namespace storage
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_STORAGE_BIT_INDEX_STORAGE_HPP_
