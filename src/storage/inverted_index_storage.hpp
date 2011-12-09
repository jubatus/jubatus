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
#include "../common/type.hpp"
#include "../common/key_manager.hpp"
#include "index_storage.hpp"
#include "sparse_matrix_storage.hpp"


namespace jubatus {
namespace storage{

class inverted_index_storage {
public:
  inverted_index_storage();
  ~inverted_index_storage();

  void set(const std::string& row, const std::string& column, float val); 

  void remove(const std::string& row, const std::string& column);
  void clear();

  void calc_scores(const sfv_t& sfv, pfi::data::unordered_map<std::string, float>& scores) const;

  void get_diff(std::string& diff_str) const;
  void set_mixed_and_clear_diff(const std::string& mixed_diff);
  void mix(const std::string& lhs_str, std::string& rhs_str) const;

  std::string name() const;

  bool save(std::ostream& os);
  bool load(std::istream& is);

private:
  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    MEMBER(inv_);
  }

  void add_inp_scores(const std::string& row, float val, 
                      pfi::data::unordered_map<uint64_t, float>& scores) const;
  
  tbl_t inv_;
  tbl_t inv_diff_;
  key_manager column2id_;
};

}
}
