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

namespace jubatus {
namespace recommender {

class minhash : public recommender_base {
public:
  minhash(pfi::lang::shared_ptr<storage::recommender_storage> recommender_storage_ptr);
  ~minhash();

  void similar_row(const sfv_t& query, std::vector<std::pair<std::string, float> > & ids, size_t ret_num) const;
  void clear();
  void clear_row(const std::string& id);
  void update_row(const std::string& id, const sfv_diff_t& diff);

private:
  void calc_minhash_values(const sfv_t& sfv, std::vector<float>& values) const;

  void converte2hash(const sfv_t& sfv, std::vector<uint64_t>& ids);
  float calc_resemble(const std::pair<float, std::vector<uint64_t> >& ids1,
                      const std::pair<float, std::vector<uint64_t> >& ids2);

  static float calc_hash(uint64_t a, uint64_t b, float val);
  static void hash_mix64(uint64_t& a, uint64_t& b, uint64_t& c);

  static const uint64_t hash_prime;
  uint64_t bit_num_;
  uint64_t hash_num_;
  std::vector<std::vector<std::vector<uint64_t> > > minhash_list_; // minhash_list[k][hashval][id_list]
};

} // namespace recommender
} // namespace jubatus
