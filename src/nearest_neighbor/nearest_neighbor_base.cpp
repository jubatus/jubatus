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

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "nearest_neighbor_base.hpp"

#include "exception.hpp"

using namespace std;

namespace jubatus {
namespace nearest_neighbor {

nearest_neighbor_base::nearest_neighbor_base(
    table::column_table* table,
    const std::string& id)
    : my_id_(id), table_(table) {}

void nearest_neighbor_base::get_all_row_ids(vector<string>& ids) const {
  vector<string> ret;
  ret.reserve(table_->size());
  for (size_t i = 0; i < table_->size(); ++i) {
    ret.push_back(table_->get_key(i));
  }
  ret.swap(ids);
}

void nearest_neighbor_base::clear() {
  // TODO: Implement clear in column_table
}

void nearest_neighbor_base::similar_row(const sfv_t& query,
                                        vector<pair<string, float> >& ids,
                                        uint64_t ret_num) const {
  neighbor_row(query, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = calc_similarity(ids[i].second);
  }
}

void nearest_neighbor_base::similar_row(const string& query_id,
                                        vector<pair<string, float> >& ids,
                                        uint64_t ret_num) const {
  neighbor_row(query_id, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = calc_similarity(ids[i].second);
  }
}

void nearest_neighbor_base::save(ostream& os) const {
  table_->save(os);
}

void nearest_neighbor_base::load(istream& is) {
  table_->load(is);
}

}  // namespace nearest_neighbor
}  // namespace jubatus
