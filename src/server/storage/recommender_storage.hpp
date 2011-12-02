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
#include "../../common/key_manager.hpp"

namespace jubatus {
namespace storage{

class recommender_storage {
public:
  recommender_storage();
  ~recommender_storage();

  void set(const std::string& row, const std::string& column, float val); 
  void set_row(const std::string& row, const std::vector<std::pair<std::string, float> >& columns);
  float get(const std::string& row, const std::string& column) const;
  void get_row(const std::string& row, std::vector<std::pair<std::string, float> >& columns) const;
  void get_all_row_ids(std::vector<std::string>& ids) const;
  void clear();

private:
  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(tbl_)
      & MEMBER(column2id_);
  }

  key_manager column2id_;
  typedef std::vector<std::pair<uint64_t, float> > column_t;
  typedef pfi::data::unordered_map<std::string, column_t> tbl_t;
  tbl_t tbl_;
};

}
}
