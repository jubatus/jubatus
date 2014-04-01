// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_RECOMMENDER_NEAREST_NEIGHBOR_RECOMMENDER_HPP_
#define JUBATUS_CORE_RECOMMENDER_NEAREST_NEIGHBOR_RECOMMENDER_HPP_

#include <string>
#include <utility>
#include <vector>
#include "jubatus/util/lang/shared_ptr.h"
#include "../nearest_neighbor/nearest_neighbor_base.hpp"
#include "../unlearner/unlearner_base.hpp"
#include "recommender_base.hpp"

namespace jubatus {
namespace core {
namespace recommender {

// Recommender based on nearest_neighbor algorithms.
class nearest_neighbor_recommender : public recommender_base {
 public:
  explicit nearest_neighbor_recommender(
      jubatus::util::lang::shared_ptr<nearest_neighbor::nearest_neighbor_base>
      nearest_neighbor_engine);

  nearest_neighbor_recommender(
      jubatus::util::lang::shared_ptr<nearest_neighbor::nearest_neighbor_base>
          nearest_neighbor_engine,
      jubatus::util::lang::shared_ptr<unlearner::unlearner_base> unlearner);

  void similar_row(
      const common::sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  void neighbor_row(
      const common::sfv_t& query,
      std::vector<std::pair<std::string, float> >& ids,
      size_t ret_num) const;
  void clear();
  void clear_row(const std::string& id);
  void update_row(const std::string& id, const sfv_diff_t& diff);
  void get_all_row_ids(std::vector<std::string>& ids) const;

  std::string type() const;

  void register_mixables_to_holder(framework::mixable_holder& holder) const;

 private:
  jubatus::util::lang::shared_ptr<table::column_table> get_table();
  jubatus::util::lang::shared_ptr<const table::column_table> get_const_table() const;

  jubatus::util::lang::shared_ptr<unlearner::unlearner_base> get_unlearner();

 private:
  class unlearning_callback;

  jubatus::util::lang::shared_ptr<nearest_neighbor::nearest_neighbor_base>
      nearest_neighbor_engine_;
  jubatus::util::lang::shared_ptr<unlearner::unlearner_base> unlearner_;
};

}  // namespace recommender
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_RECOMMENDER_NEAREST_NEIGHBOR_RECOMMENDER_HPP_
