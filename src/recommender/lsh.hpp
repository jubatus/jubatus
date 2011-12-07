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

#include "recommender_base.hpp"
#include "bit_vector.hpp"

namespace jubatus {
namespace recommender {

class lsh : public recommender_base {
public:
  lsh(pfi::lang::shared_ptr<storage::recommender_storage> storage, uint64_t base_num);
  lsh(pfi::lang::shared_ptr<storage::recommender_storage> storage);
  ~lsh();

  void similar_row(const sfv_t& query, std::vector<std::pair<std::string, float> > & ids, size_t ret_num) const;
  void clear();
  void clear_row(const std::string& id);
  void update_row(const std::string& id, const sfv_diff_t& diff);

private:
  typedef std::set<std::pair<float, std::string> > sorted_ids_t;
  void calc_lsh_values(const sfv_t& sfv, bit_vector& bv) const;
  void generate_column_base(const std::string& column);

  pfi::data::unordered_map<std::string, std::vector<float> > column2baseval_; // bases for lsh
  pfi::data::unordered_map<std::string, bit_vector > row2lshvals_;

  uint64_t base_num_;
};

} // namespace recommender
} // namespace jubatus
