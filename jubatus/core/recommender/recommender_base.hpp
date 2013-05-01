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

#ifndef JUBATUS_CORE_RECOMMENDER_RECOMMENDER_BASE_HPP_
#define JUBATUS_CORE_RECOMMENDER_RECOMMENDER_BASE_HPP_

#include <vector>
#include <string>
#include <utility>
#include <pficommon/data/unordered_map.h>
#include <pficommon/lang/shared_ptr.h>
#include "../common/type.hpp"
#include "../storage/sparse_matrix_storage.hpp"
#include "../storage/recommender_storage_base.hpp"
#include "recommender_type.hpp"

namespace jubatus {
namespace core {
namespace recommender {

class recommender_base {
 public:
  recommender_base();
  virtual ~recommender_base();

  // return similar row for query with similarity scores.
  virtual void similar_row(
      const sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const = 0;
  // return similar row for query with distance scores.
  virtual void neighbor_row(
      const sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const = 0;
  virtual void clear() = 0;
  virtual void clear_row(const std::string& id) = 0;
  virtual void update_row(const std::string& id, const sfv_diff_t& diff) = 0;
  virtual void get_all_row_ids(std::vector<std::string>& ids) const = 0;

  virtual std::string type() const = 0;
  virtual core::storage::recommender_storage_base* get_storage() = 0;
  virtual const core::storage::recommender_storage_base* get_const_storage()
      const = 0;

  virtual void similar_row(
      const std::string& id,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  virtual void neighbor_row(
      const std::string& id,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  void complete_row(const std::string& id, sfv_t& ret) const;
  void complete_row(const sfv_t& query, sfv_t& ret) const;
  void decode_row(const std::string& id, sfv_t& ret) const;

  void save(std::ostream&);
  void load(std::istream&);

  static float calc_similality(sfv_t& q1, sfv_t& q2);
  static float calc_l2norm(const sfv_t& query);

 protected:
  virtual bool save_impl(std::ostream&) = 0;
  virtual bool load_impl(std::istream&) = 0;

  static const uint64_t complete_row_similar_num_;
  core::storage::sparse_matrix_storage orig_;
};

}  // namespace recommender
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_RECOMMENDER_RECOMMENDER_BASE_HPP_
