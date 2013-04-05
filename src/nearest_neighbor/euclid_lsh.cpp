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

#include "euclid_lsh.hpp"

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <cmath>
#include <pficommon/lang/cast.h>
#include "../common/config_util.hpp"
#include "../storage/fixed_size_heap.hpp"
#include "lsh_function.hpp"

using std::map;
using std::pair;
using std::make_pair;
using std::string;
using std::vector;
using pfi::lang::lexical_cast;
using jubatus::table::column_table;
using jubatus::table::column_type;
using jubatus::table::owner;
using jubatus::table::bit_vector;
using jubatus::table::const_bit_vector_column;
using jubatus::table::const_float_column;

namespace jubatus {
namespace nearest_neighbor {

namespace {
float squared_l2norm(const sfv_t& sfv) {
  float sqnorm = 0;
  for (size_t i = 0; i < sfv.size(); ++i) {
    sqnorm += sfv[i].second * sfv[i].second;
  }
  return sqnorm;
}

float l2norm(const sfv_t& sfv) {
  return sqrt(squared_l2norm(sfv));
}

float calc_euclidean_distance(size_t hash_num,
                              size_t hamming_similarity,
                              float norm1,
                              float norm2) {
  if (hash_num == hamming_similarity) {
    return abs(norm1 - norm2);
  }
  const float theta = (hash_num - hamming_similarity) * M_PI / hash_num;
  return norm1 * (norm1 - 2 * norm2 * cos(theta));
}

}  // namespace

euclid_lsh::euclid_lsh(const map<string, string>& config,
                       column_table* table, const std::string& id)
  : nearest_neighbor_base(table, id) {
  set_config(config);

  vector<column_type> schema;
  fill_schema(schema);
  get_table()->init(schema);
}

euclid_lsh::euclid_lsh(const map<string, string>& config,
                       column_table* table,
                       vector<column_type>& schema,
                       const std::string& id)
  : nearest_neighbor_base(table, id) {
  set_config(config);
  fill_schema(schema);
}

void euclid_lsh::set_row(const string& id, const sfv_t& sfv) {
  // TODO: support nested algorithm, e.g. when used by lof and then we cannot
  // suppose that the first two columns are assigned to euclid_lsh.
  get_table()->add(id, owner(my_id_), cosine_lsh(sfv, hash_num_), l2norm(sfv));
}

void euclid_lsh::neighbor_row(const sfv_t& query,
                              vector<pair<string, float> >& ids,
                              uint64_t ret_num) const {
  neighbor_row_from_hash(
      cosine_lsh(query, hash_num_),
      l2norm(query),
      ids,
      ret_num);
}

void euclid_lsh::neighbor_row(const std::string& query_id,
                              vector<pair<string, float> >& ids,
                              uint64_t ret_num) const {
  const pair<bool, uint64_t> maybe_index
      = get_const_table()->exact_match(query_id);
  if (!maybe_index.first) {
    ids.clear();
    return;
  }

  const bit_vector bv = lsh_column()[maybe_index.second];
  const float norm = norm_column()[maybe_index.second];
  neighbor_row_from_hash(bv, norm, ids, ret_num);
}

void euclid_lsh::set_config(const map<string, string>& config) {
  hash_num_ = get_param(config, "euclid_lsh:hash_num", 64u);
}

void euclid_lsh::fill_schema(vector<column_type>& schema) {
  first_column_id_ = schema.size();
  schema.push_back(column_type(column_type::bit_vector_type, hash_num_));
  schema.push_back(column_type(column_type::float_type));
}

const_bit_vector_column euclid_lsh::lsh_column() const {
  return get_const_table()->get_bit_vector_column(first_column_id_);
}

const_float_column euclid_lsh::norm_column() const {
  return get_const_table()->get_float_column(first_column_id_ + 1);
}

void euclid_lsh::neighbor_row_from_hash(const bit_vector& bv,
                                        float norm,
                                        vector<pair<string, float> >& ids,
                                        uint64_t ret_num) const {
  const column_table* table = get_const_table();

  storage::fixed_size_heap<pair<float, size_t> > heap(ret_num);
  {
    const_bit_vector_column bv_col = lsh_column();
    const_float_column norm_col = norm_column();

    const float denom = bv.bit_num();
    for (size_t i = 0; i < table->size(); ++i) {
      const size_t hamm_dist = bv.calc_hamming_distance(bv_col[i]);
      const float theta = hamm_dist * M_PI / denom;
      const float score = norm_col[i] * (norm_col[i] - 2 * norm * cos(theta));
      heap.push(make_pair(score, i));
    }
  }

  vector<pair<float, size_t> > sorted;
  heap.get_sorted(sorted);

  ids.clear();
  const float squared_norm = norm * norm;
  for (size_t i = 0; i < sorted.size(); ++i) {
    ids.push_back(make_pair(table->get_key(sorted[i].second),
                            sqrt(squared_norm + sorted[i].first)));
  }
}

}  // namespace nearest_neighbor
}  // namespace jubatus
