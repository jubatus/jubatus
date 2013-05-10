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

#ifndef JUBATUS_CORE_STORAGE_INVERTED_INDEX_STORAGE_HPP_
#define JUBATUS_CORE_STORAGE_INVERTED_INDEX_STORAGE_HPP_

#include <string>
#include <utility>
#include <vector>
#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/unordered_map.h>
#include "storage_type.hpp"
#include "../common/type.hpp"
#include "../common/key_manager.hpp"
#include "sparse_matrix_storage.hpp"
#include "recommender_storage_base.hpp"

namespace jubatus {
namespace core {
namespace storage {

class inverted_index_storage : public recommender_storage_base {
 public:
  inverted_index_storage();
  ~inverted_index_storage();

  void set(const std::string& row, const std::string& column, float val);
  float get(const std::string& row, const std::string& column) const;
  void remove(const std::string& row, const std::string& column);
  void clear();
  void get_all_column_ids(std::vector<std::string>& ids) const;

  void calc_scores(
      const sfv_t& sfv,
      std::vector<std::pair<std::string, float> >& scores,
      size_t ret_num) const;

  void get_diff(std::string& diff_str) const;
  void set_mixed_and_clear_diff(const std::string& mixed_diff);
  void mix(const std::string& lhs_str, std::string& rhs_str) const;

  std::string name() const;

  bool save(std::ostream& os);
  bool load(std::istream& is);

 private:
  static float calc_l2norm(const sfv_t& sfv);
  float calc_columnl2norm(uint64_t column_id) const;
  float get_from_tbl(
      const std::string& row,
      uint64_t column_id,
      const tbl_t& tbl,
      bool& exist) const;

  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(inv_) & MEMBER(inv_diff_) & MEMBER(column2norm_)
      & MEMBER(column2norm_diff_) & MEMBER(column2id_);
  }

  void add_inp_scores(
      const std::string& row,
      float val,
      pfi::data::unordered_map<uint64_t, float>& scores) const;

  tbl_t inv_;
  tbl_t inv_diff_;
  imap_float_t column2norm_;
  imap_float_t column2norm_diff_;
  key_manager column2id_;
};

}  // namespace storage
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_STORAGE_INVERTED_INDEX_STORAGE_HPP_
