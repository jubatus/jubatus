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

#ifndef JUBATUS_CORE_RECOMMENDER_LSH_HPP_
#define JUBATUS_CORE_RECOMMENDER_LSH_HPP_

#include <string>
#include <utility>
#include <vector>
#include <pficommon/data/serialization.h>

#include "recommender_base.hpp"
#include "../storage/bit_index_storage.hpp"

namespace jubatus {
namespace core {
namespace recommender {

class lsh : public recommender_base {
 public:
  struct config {
    config();

    int64_t bit_num;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & MEMBER(bit_num);
    }
  };

  explicit lsh(uint64_t base_num);
  explicit lsh(const config& config);
  lsh();
  ~lsh();

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

  void calc_lsh_values(const sfv_t& sfv, core::storage::bit_vector& bv) const;
  void generate_column_base(const std::string& column);
  void generate_column_bases(const sfv_t& v);

  // bases for lsh
  pfi::data::unordered_map<std::string, std::vector<float> > column2baseval_;

  core::storage::bit_index_storage row2lshvals_;

  const uint64_t base_num_;
};

}  // namespace recommender
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_RECOMMENDER_LSH_HPP_
