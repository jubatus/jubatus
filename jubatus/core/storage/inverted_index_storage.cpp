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

#include "inverted_index_storage.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using std::istringstream;
using std::make_pair;
using std::ostringstream;
using std::pair;
using std::sort;
using std::sqrt;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {

inverted_index_storage::inverted_index_storage() {
}

inverted_index_storage::~inverted_index_storage() {
}

void inverted_index_storage::set(
    const std::string& row,
    const std::string& column,
    float val) {
  uint64_t column_id = column2id_.get_id_const(column);

  if (column_id == common::key_manager::NOTFOUND) {
    column_id = column2id_.get_id(column);
  } else {
    float cur_val = get(row, column);
    column2norm_diff_[column_id] -= cur_val * cur_val;
  }
  inv_diff_[row][column_id] = val;
  column2norm_diff_[column_id] += val * val;
}

float inverted_index_storage::get(
    const string& row,
    const string& column) const {
  uint64_t column_id = column2id_.get_id_const(column);
  if (column_id == common::key_manager::NOTFOUND) {
    return 0.f;
  }
  {
    bool exist = false;
    float ret = get_from_tbl(row, column_id, inv_diff_, exist);
    if (exist) {
      return ret;
    }
  }
  {
    bool exist = false;
    float ret = get_from_tbl(row, column_id, inv_, exist);
    if (exist) {
      return ret;
    }
  }
  return 0.0;
}

float inverted_index_storage::get_from_tbl(
    const std::string& row,
    uint64_t column_id,
    const tbl_t& tbl,
    bool& exist) const {
  exist = false;

  if (column_id == common::key_manager::NOTFOUND) {
    return 0.f;
  }
  tbl_t::const_iterator it = tbl.find(row);
  if (it == tbl.end()) {
    return 0.f;
  } else {
    row_t::const_iterator it_row = it->second.find(column_id);
    if (it_row == it->second.end()) {
      return 0.f;
    } else {
      exist = true;
      return it_row->second;
    }
  }
}

void inverted_index_storage::remove(
    const std::string& row,
    const std::string& column) {
  set(row, column, 0.f);
}

void inverted_index_storage::clear() {
  tbl_t().swap(inv_);
  tbl_t().swap(inv_diff_);
  imap_float_t().swap(column2norm_);
  imap_float_t().swap(column2norm_diff_);
  common::key_manager().swap(column2id_);
}

void inverted_index_storage::get_all_column_ids(
    std::vector<std::string>& ids) const {
  ids.clear();
  for (imap_float_t::const_iterator it = column2norm_.begin();
      it != column2norm_.end(); ++it) {
    ids.push_back(column2id_.get_key(it->first));
  }
  for (imap_float_t::const_iterator it = column2norm_diff_.begin();
      it != column2norm_diff_.end(); ++it) {
    if (column2norm_.find(it->first) == column2norm_.end()) {
      ids.push_back(column2id_.get_key(it->first));
    }
  }
}

void inverted_index_storage::get_diff(diff_type& diff) const {
  for (tbl_t::const_iterator it = inv_diff_.begin(); it != inv_diff_.end();
      ++it) {
    vector<pair<string, float> > columns;
    for (row_t::const_iterator it2 = it->second.begin();
        it2 != it->second.end(); ++it2) {
      columns.push_back(make_pair(column2id_.get_key(it2->first), it2->second));
    }
    diff.inv.set_row(it->first, columns);
  }

  for (imap_float_t::const_iterator it = column2norm_diff_.begin();
      it != column2norm_diff_.end(); ++it) {
    diff.column2norm[column2id_.get_key(it->first)] = it->second;
  }
}

void inverted_index_storage::set_mixed_and_clear_diff(
    const diff_type& mixed_diff) {
  vector<string> ids;
  mixed_diff.inv.get_all_row_ids(ids);
  for (size_t i = 0; i < ids.size(); ++i) {
    const string& row = ids[i];
    row_t& v = inv_[row];
    vector<pair<string, float> > columns;
    mixed_diff.inv.get_row(row, columns);
    for (size_t j = 0; j < columns.size(); ++j) {
      size_t id = column2id_.get_id(columns[j].first);
      if (columns[j].second == 0.f) {
        v.erase(id);
      } else {
        v[id] = columns[j].second;
      }
    }
  }
  inv_diff_.clear();

  for (map_float_t::const_iterator it = mixed_diff.column2norm.begin();
      it != mixed_diff.column2norm.end(); ++it) {
    uint64_t column_index = column2id_.get_id(it->first);
    column2norm_[column_index] += it->second;
    if (column2norm_[column_index] == 0.f) {
      column2norm_.erase(column_index);
    }
  }
  column2norm_diff_.clear();
}

void inverted_index_storage::mix(const diff_type& lhs, diff_type& rhs) const {
  // merge inv diffs
  vector<string> ids;
  lhs.inv.get_all_row_ids(ids);
  for (size_t i = 0; i < ids.size(); ++i) {
    const string& row = ids[i];

    vector<pair<string, float> > columns;
    lhs.inv.get_row(row, columns);
    rhs.inv.set_row(row, columns);
  }

  // merge norm diffs
  for (map_float_t::const_iterator it = lhs.column2norm.begin();
      it != lhs.column2norm.end(); ++it) {
    rhs.column2norm[it->first] += it->second;
  }
}

void inverted_index_storage::pack(
    msgpack::packer<msgpack::sbuffer>& packer) const {
  packer.pack(*this);
}

void inverted_index_storage::unpack(msgpack::object o) {
  o.convert(this);
}

void inverted_index_storage::calc_scores(
    const common::sfv_t& query,
    vector<pair<string, float> >& scores,
    size_t ret_num) const {
  float query_norm = calc_l2norm(query);
  if (query_norm == 0.f) {
    return;
  }
  pfi::data::unordered_map<uint64_t, float> i_scores;
  for (size_t i = 0; i < query.size(); ++i) {
    const string& fid = query[i].first;
    float val = query[i].second;
    add_inp_scores(fid, val, i_scores);
  }

  vector<pair<float, uint64_t> > sorted_scores;
  for (pfi::data::unordered_map<uint64_t, float>::const_iterator it = i_scores
      .begin(); it != i_scores.end(); ++it) {
    float norm = calc_columnl2norm(it->first);
    float normed_score = (norm != 0.f) ? it->second / norm / query_norm : 0.f;
    sorted_scores.push_back(make_pair(normed_score, it->first));
  }
  sort(sorted_scores.rbegin(), sorted_scores.rend());
  for (size_t i = 0; i < sorted_scores.size() && i < ret_num; ++i) {
    scores.push_back(
        make_pair(column2id_.get_key(sorted_scores[i].second),
                  sorted_scores[i].first));
  }
}

float inverted_index_storage::calc_l2norm(const common::sfv_t& sfv) {
  float ret = 0.f;
  for (size_t i = 0; i < sfv.size(); ++i) {
    ret += sfv[i].second * sfv[i].second;
  }
  return sqrt(ret);
}

float inverted_index_storage::calc_columnl2norm(uint64_t column_id) const {
  float ret = 0.f;
  imap_float_t::const_iterator it_diff = column2norm_diff_.find(column_id);
  if (it_diff != column2norm_diff_.end()) {
    ret += it_diff->second;
  }
  imap_float_t::const_iterator it = column2norm_.find(column_id);
  if (it != column2norm_.end()) {
    ret += it->second;
  }
  return sqrt(ret);
}

void inverted_index_storage::add_inp_scores(
    const std::string& row,
    float val,
    pfi::data::unordered_map<uint64_t, float>& scores) const {
  tbl_t::const_iterator it_diff = inv_diff_.find(row);
  if (it_diff != inv_diff_.end()) {
    const row_t& row_v = it_diff->second;
    for (row_t::const_iterator row_it = row_v.begin(); row_it != row_v.end();
        ++row_it) {
      scores[row_it->first] += row_it->second * val;
    }
  }

  tbl_t::const_iterator it = inv_.find(row);
  if (it != inv_.end()) {
    const row_t& row_v = it->second;
    if (it_diff == inv_diff_.end()) {
      for (row_t::const_iterator row_it = row_v.begin(); row_it != row_v.end();
          ++row_it) {
        scores[row_it->first] += row_it->second * val;
      }
    } else {
      const row_t& row_diff_v = it_diff->second;
      for (row_t::const_iterator row_it = row_v.begin(); row_it != row_v.end();
          ++row_it) {
        if (row_diff_v.find(row_it->first) == row_diff_v.end()) {
          scores[row_it->first] += row_it->second * val;
        }
      }
    }
  }
}

std::string inverted_index_storage::name() const {
  return string("inverted_index_storage");
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
