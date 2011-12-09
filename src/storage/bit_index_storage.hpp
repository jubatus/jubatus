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

#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/unordered_map.h>
#include "../common/key_manager.hpp"
#include "storage_type.hpp"
#include "sparse_matrix_storage.hpp"
#include "bit_vector.hpp"

namespace jubatus {
namespace storage{

class bit_index_storage {
public:
  bit_index_storage();
  ~bit_index_storage();

  void set_row(const std::string& row, const bit_vector& bv);
  void remove_row(const std::string& row);
  void clear();

  void get_diff(sparse_matrix_storage& sms) const;
  void set_mixed_and_clear_diff(sparse_matrix_storage& mixed) const;

  void similar_row(const bit_vector& bv, std::vector<std::pair<std::string, float> >& ids, uint64_t ret_num) const;
  std::string name() const;

  bool save(std::ostream& os);
  bool load(std::istream& is);

private:
  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(row2bitvals_);
  }
  
  pfi::data::unordered_map<std::string, bit_vector> row2bitvals_;
};

}
}
