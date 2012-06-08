// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "recommender_base.hpp"
#include "../storage/recommender_storage.hpp"

namespace jubatus {
namespace recommender {

class inverted_index : public recommender_base {
public:
  inverted_index();
  ~inverted_index();

  void similar_row(const sfv_t& query, std::vector<std::pair<std::string, float> > & ids, size_t ret_num) const;
  void clear();
  void clear_row(const std::string& id);
  void update_row(const std::string& id, const sfv_diff_t& diff);
  void get_all_row_ids(std::vector<std::string>& ids) const;
  std::string type() const;
  storage::recommender_storage_base* get_storage();
  const storage::recommender_storage_base* get_const_storage() const;

private:
  bool save_impl(std::ostream&);
  bool load_impl(std::istream&);
  storage::inverted_index_storage inv_;
};

} // namespace recommender
} // namespace jubatus
