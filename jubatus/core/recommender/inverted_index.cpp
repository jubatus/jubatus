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

inverted_index::inverted_index()
    : mixable_storage_(new storage::mixable_inverted_index_storage) {
  mixable_storage_->set_model(
      pfi::lang::shared_ptr<storage::inverted_index_storage>(
          new storage::inverted_index_storage));
}

inverted_index::~inverted_index() {
}

void inverted_index::similar_row(
    const common::sfv_t& query,
    std::vector<std::pair<std::string, float> >& ids,
    size_t ret_num) const {
  ids.clear();
  if (ret_num == 0) {
    return;
  }
  mixable_storage_->get_model()->calc_scores(query, ids, ret_num);
}

void inverted_index::neighbor_row(
    const common::sfv_t& query,
    vector<pair<string, float> >& ids,
    size_t ret_num) const {
  similar_row(query, ids, ret_num);
  for (size_t i = 0; i < ids.size(); ++i) {
    ids[i].second = 1 - ids[i].second;
  }
}

void inverted_index::clear() {
  orig_.clear();
  mixable_storage_->get_model()->clear();
}

void inverted_index::clear_row(const std::string& id) {
  vector<pair<string, float> > columns;
  orig_.get_row(id, columns);
  storage::inverted_index_storage& inv = *mixable_storage_->get_model();
  for (size_t i = 0; i < columns.size(); ++i) {
    inv.remove(columns[i].first, id);
  }
  orig_.remove_row(id);
}

void inverted_index::update_row(const std::string& id, const sfv_diff_t& diff) {
  orig_.set_row(id, diff);
  storage::inverted_index_storage& inv = *mixable_storage_->get_model();
  for (size_t i = 0; i < diff.size(); ++i) {
    inv.set(diff[i].first, id, diff[i].second);
  }
}

void inverted_index::get_all_row_ids(std::vector<std::string>& ids) const {
  mixable_storage_->get_model()->get_all_column_ids(ids);  // inv.column = row
}

string inverted_index::type() const {
  return string("inverted_index");
}

void inverted_index::pack_impl(
    msgpack::packer<msgpack::sbuffer>& packer) const {
  mixable_storage_->pack(packer);
}

void inverted_index::unpack_impl(msgpack::object o) {
  mixable_storage_->unpack(o);
}

void inverted_index::register_mixables_to_holder(
    framework::mixable_holder& holder) const {
  holder.register_mixable(mixable_storage_);
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus
