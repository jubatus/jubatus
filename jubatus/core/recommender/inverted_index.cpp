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

#include "inverted_index.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>
#include "../common/exception.hpp"
#include "../common/vector_util.hpp"

using std::pair;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace recommender {

inverted_index::inverted_index() {
}

inverted_index::~inverted_index() {
}

void inverted_index::similar_row(
    const sfv_t& query,
    std::vector<std::pair<std::string, float> >& ids,
    size_t ret_num) const {
  ids.clear();
  if (ret_num == 0) {
    return;
  }
  inv_.calc_scores(query, ids, ret_num);
}

void inverted_index::neighbor_row(
    const sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  similar_row(query, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = 1 - ids[i].second;
  }
}

void inverted_index::clear() {
  orig_.clear();
  inv_.clear();
}

void inverted_index::clear_row(const std::string& id) {
  vector<pair<string, float> > columns;
  orig_.get_row(id, columns);
  for (size_t i = 0; i < columns.size(); ++i) {
    inv_.remove(columns[i].first, id);
  }
  orig_.remove_row(id);
}

void inverted_index::update_row(const std::string& id, const sfv_diff_t& diff) {
  orig_.set_row(id, diff);
  for (size_t i = 0; i < diff.size(); ++i) {
    inv_.set(diff[i].first, id, diff[i].second);
  }
}

void inverted_index::get_all_row_ids(std::vector<std::string>& ids) const {
  inv_.get_all_column_ids(ids);  // inv_.column = row
}

string inverted_index::type() const {
  return string("inverted_index");
}

bool inverted_index::save_impl(std::ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << inv_;
  return true;
}

bool inverted_index::load_impl(std::istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> inv_;
  return true;
}

core::storage::recommender_storage_base* inverted_index::get_storage() {
  return &inv_;
}

const core::storage::recommender_storage_base*
    inverted_index::get_const_storage()
     const {
  return &inv_;
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
