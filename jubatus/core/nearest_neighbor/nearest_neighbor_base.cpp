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

#include <string>
#include <utility>
#include <vector>

#include "exception.hpp"
#include "nearest_neighbor_base.hpp"

using std::pair;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace nearest_neighbor {

nearest_neighbor_base::nearest_neighbor_base(
    pfi::lang::shared_ptr<table::column_table> table,
    const std::string& id)
    : my_id_(id),
      mixable_table_(new driver::mixable_versioned_table) {
  mixable_table_->set_model(table);
}

void nearest_neighbor_base::get_all_row_ids(vector<string>& ids) const {
  vector<string> ret;
  pfi::lang::shared_ptr<const table::column_table> table = get_const_table();
  ret.reserve(table->size());
  for (size_t i = 0; i < table->size(); ++i) {
    ret.push_back(table->get_key(i));
  }
  ret.swap(ids);
}

void nearest_neighbor_base::clear() {
  // TODO(beam2d): Implement clear in column_table
}

void nearest_neighbor_base::similar_row(
    const common::sfv_t& query,
    vector<pair<string, float> >& ids,
    uint64_t ret_num) const {
  neighbor_row(query, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = calc_similarity(ids[i].second);
  }
}

void nearest_neighbor_base::similar_row(
    const string& query_id,
    vector<pair<string, float> >& ids,
    uint64_t ret_num) const {
  neighbor_row(query_id, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = calc_similarity(ids[i].second);
  }
}

void nearest_neighbor_base::save(std::ostream& os) const {
  get_const_table()->save(os);
}

void nearest_neighbor_base::load(std::istream& is) {
  get_table()->load(is);
}

void nearest_neighbor_base::register_mixables(
    framework::mixable_holder& holder) const {
  if (mixable_table_) {
    holder.register_mixable(mixable_table_.get());
  }
}

}  // namespace nearest_neighbor
}  // namespcae core
}  // namespace jubatus
