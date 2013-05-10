// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_RECOMMENDER_MINHASH_HPP_
#define JUBATUS_CORE_RECOMMENDER_MINHASH_HPP_

#include <string>
#include <utility>
#include <vector>
#include <pficommon/data/serialization.h>

#include "recommender_base.hpp"
#include "../storage/bit_index_storage.hpp"

namespace jubatus {
namespace core {
namespace recommender {

class minhash : public recommender_base {
 public:
  struct config {
    config()
        : hash_num(64) {
    }

    int64_t hash_num;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & MEMBER(hash_num);
    }
  };

  minhash();
  explicit minhash(const config& config);
  ~minhash();

  void similar_row(
      const sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  void neighbor_row(
      const sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  void clear();
  void clear_row(const std::string& id);
  void update_row(const std::string& id, const sfv_diff_t& diff);
  void get_all_row_ids(std::vector<std::string>& ids) const;
  std::string type() const;
  core::storage::recommender_storage_base* get_storage();
  const core::storage::recommender_storage_base* get_const_storage() const;

 private:
  bool save_impl(std::ostream&);
  bool load_impl(std::istream&);

  void calc_minhash_values(
      const sfv_t& sfv,
      core::storage::bit_vector& bv) const;

  static float calc_hash(uint64_t a, uint64_t b, float val);
  static void hash_mix64(uint64_t& a, uint64_t& b, uint64_t& c);

  static const uint64_t hash_prime;
  uint64_t hash_num_;
  core::storage::bit_index_storage row2minhashvals_;
};

}  // namespace recommender
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_RECOMMENDER_MINHASH_HPP_
