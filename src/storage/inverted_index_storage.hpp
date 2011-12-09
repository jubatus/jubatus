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
#include "storage_type.hpp"
#include "index_storage.hpp"
#include "sparse_matrix_storage.hpp"
#include "../common/key_manager.hpp"

namespace jubatus {
namespace storage{

class inverted_index_storage {
public:
  inverted_index_storage();
  ~inverted_index_storage();

  void set(const std::string& row, const std::string& column, float val); 
  void get_row(const std::string& row, std::vector<std::pair<std::string, float> >& columns) const;

  void remove(const std::string& row, const std::string& column);
  void clear();

  void get_diff(sparse_matrix_storage& sms) const;
  void set_mixed_and_clear_diff(sparse_matrix_storage& mixed) const;

  std::string name() const;

private:
  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    MEMBER(inv_);
  }
  
  sparse_matrix_storage inv_;
};

}
}
