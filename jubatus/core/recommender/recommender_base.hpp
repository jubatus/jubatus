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
#include "jubatus/util/data/unordered_map.h"
#include "jubatus/util/lang/shared_ptr.h"
#include "../table/column/column_table.hpp"
#include "../common/type.hpp"
#include "../framework/mixable.hpp"
#include "../storage/sparse_matrix_storage.hpp"
#include "../storage/recommender_storage_base.hpp"
#include "../unlearner/unlearner_base.hpp"
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
      const common::sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const = 0;
  // return similar row for query with distance scores.
  virtual void neighbor_row(
      const common::sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const = 0;
  virtual void clear() = 0;
  virtual void clear_row(const std::string& id) = 0;
  virtual void update_row(const std::string& id, const sfv_diff_t& diff) = 0;
  virtual void get_all_row_ids(std::vector<std::string>& ids) const = 0;

  virtual std::string type() const = 0;

  // Either get_(const)_storage or get_(const)_table should return non-null
  // pointer.
  virtual core::storage::recommender_storage_base* get_storage() {
    return NULL;
  }
  virtual const core::storage::recommender_storage_base* get_const_storage()
      const {
    return NULL;
  }

  virtual jubatus::util::lang::shared_ptr<table::column_table> get_table() {
    return jubatus::util::lang::shared_ptr<table::column_table>();
  }
  virtual jubatus::util::lang::shared_ptr<const table::column_table> get_const_table()
      const {
    return jubatus::util::lang::shared_ptr<const table::column_table>();
  }

  virtual jubatus::util::lang::shared_ptr<unlearner::unlearner_base> get_unlearner() {
    return jubatus::util::lang::shared_ptr<unlearner::unlearner_base>();
  }

  virtual void similar_row(
      const std::string& id,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  virtual void neighbor_row(
      const std::string& id,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  void complete_row(const std::string& id, common::sfv_t& ret) const;
  void complete_row(const common::sfv_t& query, common::sfv_t& ret) const;
  void decode_row(const std::string& id, common::sfv_t& ret) const;

  virtual void register_mixables_to_holder(framework::mixable_holder& holder)
      const = 0;

  static float calc_similality(common::sfv_t& q1, common::sfv_t& q2);
  static float calc_l2norm(const common::sfv_t& query);

 protected:
  static const uint64_t complete_row_similar_num_;

  // TODO(beam2d): Workaround to correctly store the storage on save.
  util::lang::shared_ptr<core::storage::sparse_matrix_storage_mixable> orig_;
};

}  // namespace recommender
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_RECOMMENDER_RECOMMENDER_BASE_HPP_
