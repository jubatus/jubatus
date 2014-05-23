// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "bit_vector_nearest_neighbor_base.hpp"

#include <string>
#include <utility>
#include <vector>
#include "../common/type.hpp"
#include "bit_vector_ranking.hpp"

using std::string;
using std::pair;
using std::vector;
using jubatus::core::table::column_table;
using jubatus::core::table::column_type;
using jubatus::core::table::bit_vector;
using jubatus::core::table::const_bit_vector_column;
using jubatus::core::table::owner;

namespace jubatus {
namespace core {
namespace nearest_neighbor {

bit_vector_nearest_neighbor_base::bit_vector_nearest_neighbor_base(
    uint32_t bitnum,
    jubatus::util::lang::shared_ptr<table::column_table> table,
    const std::string& id)
    : nearest_neighbor_base(table, id),
      bitnum_(bitnum) {
  vector<column_type> schema;
  fill_schema(schema);
  table->init(schema);
}

bit_vector_nearest_neighbor_base::bit_vector_nearest_neighbor_base(
    uint32_t bitnum,
    jubatus::util::lang::shared_ptr<column_table> table,
    vector<column_type>& schema,
    const std::string& id)
    : nearest_neighbor_base(table, id),
      bitnum_(bitnum) {
  fill_schema(schema);
}

void bit_vector_nearest_neighbor_base::set_row(
    const string& id,
    const common::sfv_t& sfv) {
  // TODO(beam2d): support nested algorithm, e.g. when used by lof and then
  // we cannot suppose that the first column is assigned
  // to bit_vector_nearest_neighbor_base.
  get_table()->add(id, owner(my_id_), hash(sfv));
}

void bit_vector_nearest_neighbor_base::neighbor_row(
    const common::sfv_t& query,
    vector<pair<string, float> >& ids,
    uint64_t ret_num) const {
  neighbor_row_from_hash(hash(query), ids, ret_num);
}

void bit_vector_nearest_neighbor_base::neighbor_row(
    const string& query_id,
    vector<pair<string, float> >& ids,
    uint64_t ret_num) const {
  const table::column_table& table = *get_const_table();
  const pair<bool, uint64_t> maybe_index = table.exact_match(query_id);
  if (!maybe_index.first) {
    ids.clear();
    return;
  }

  const_bit_vector_column& col = bit_vector_column();
  neighbor_row_from_hash(col[maybe_index.second], ids, ret_num);
}

void bit_vector_nearest_neighbor_base::fill_schema(
    vector<column_type>& schema) {
  bit_vector_column_id_ = schema.size();
  schema.push_back(column_type(column_type::bit_vector_type, bitnum_));
}

const_bit_vector_column& bit_vector_nearest_neighbor_base::bit_vector_column()
    const {
  return get_const_table()->get_bit_vector_column(bit_vector_column_id_);
}

void bit_vector_nearest_neighbor_base::neighbor_row_from_hash(
    const bit_vector& query,
    vector<pair<string, float> >& ids,
    uint64_t ret_num) const {
  vector<pair<uint64_t, float> > scores;
  ranking_hamming_bit_vectors(query, bit_vector_column(), scores, ret_num);

  jubatus::util::lang::shared_ptr<const column_table> table = get_const_table();
  ids.clear();
  for (size_t i = 0; i < scores.size(); ++i) {
    ids.push_back(make_pair(table->get_key(scores[i].first), scores[i].second));
  }
}

}  // namespace nearest_neighbor
}  // namespace core
}  // namespace jubatus
