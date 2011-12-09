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

#include <vector>
#include <pficommon/data/unordered_map.h>
#include <pficommon/lang/shared_ptr.h>
#include "../common/type.hpp"
#include "../storage/sparse_matrix_storage.hpp"
#include "recommender_type.hpp"

namespace jubatus {
namespace recommender {

class recommender_base{
public:
  recommender_base();
  virtual ~recommender_base();

  virtual void similar_row(const sfv_t& query, std::vector<std::pair<std::string, float> > & ids, size_t ret_num) const = 0; // return similar row for query.
  virtual void clear() = 0;
  virtual void clear_row(const std::string& id) = 0;
  virtual void update_row(const std::string& id, const sfv_diff_t& diff) = 0;
  virtual std::string name() const = 0;

  void similar_row(const std::string& id, std::vector<std::pair<std::string, float> > & ids, size_t ret_num) const;
  void complete_row(const std::string& id, sfv_t& ret) const;
  void complete_row(const sfv_t& query, sfv_t& ret) const;
  void decode_row(const std::string& id, sfv_t& ret) const; 
  void get_all_row_ids(std::vector<std::string>& ids) const;

protected:
  static const uint64_t complete_row_similar_num_;
  storage::sparse_matrix_storage orig_;
};

} // namespace recommender
} // namespace jubatus
