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

#include "sparse_matrix_storage.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>
#include <pficommon/data/unordered_set.h>

using std::istream;
using std::ostream;
using std::make_pair;
using std::pair;
using std::string;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {

sparse_matrix_storage::sparse_matrix_storage() {
}

sparse_matrix_storage::~sparse_matrix_storage() {
}

sparse_matrix_storage& sparse_matrix_storage::operator =(
    const sparse_matrix_storage& sms) {
  tbl_ = sms.tbl_;
  column2id_ = sms.column2id_;
  return *this;
}

void sparse_matrix_storage::set(
    const string& row,
    const string& column,
    float val) {
  tbl_[row][column2id_.get_id(column)] = val;
}

void sparse_matrix_storage::set_row(
    const string& row,
    const vector<pair<string, float> >& columns) {
  row_t& row_v = tbl_[row];
  for (size_t i = 0; i < columns.size(); ++i) {
    float& v = row_v[column2id_.get_id(columns[i].first)];
    // norm_ptr_->notify(row, v, columns[i].second);
    v = columns[i].second;
  }
}

float sparse_matrix_storage::get(
    const string& row,
    const string& column) const {
  tbl_t::const_iterator it = tbl_.find(row);
  if (it == tbl_.end()) {
    return 0.f;
  }

  uint64_t id = column2id_.get_id_const(column);
  if (id == key_manager::NOTFOUND) {
    return 0.f;
  }

  row_t::const_iterator cit = it->second.find(id);
  if (cit == it->second.end()) {
    return 0.f;
  }
  return cit->second;
}

void sparse_matrix_storage::get_row(
    const string& row,
    vector<pair<string, float> >& columns) const {
  columns.clear();
  tbl_t::const_iterator it = tbl_.find(row);
  if (it == tbl_.end()) {
    return;
  }
  const row_t& row_v = it->second;
  for (row_t::const_iterator row_it = row_v.begin(); row_it != row_v.end();
      ++row_it) {
    columns.push_back(
        make_pair(column2id_.get_key(row_it->first), row_it->second));
  }
}

float sparse_matrix_storage::calc_l2norm(const string& row) const {
  tbl_t::const_iterator it = tbl_.find(row);
  if (it == tbl_.end()) {
    return 0.f;
  }
  float sq_norm = 0.f;
  const row_t& row_v = it->second;
  for (row_t::const_iterator row_it = row_v.begin(); row_it != row_v.end();
      ++row_it) {
    sq_norm += row_it->second * row_it->second;
  }
  return std::sqrt(sq_norm);
}

void sparse_matrix_storage::remove(const string& row, const string& column) {
  tbl_t::iterator it = tbl_.find(row);
  if (it == tbl_.end()) {
    return;
  }

  uint64_t id = column2id_.get_id_const(column);
  if (id == key_manager::NOTFOUND) {
    return;
  }

  row_t::iterator cit = it->second.find(id);
  if (cit == it->second.end()) {
    return;
  }
  // norm_ptr_->notify(row, cit->second, 0.f);
  it->second.erase(cit);
}

void sparse_matrix_storage::remove_row(const string& row) {
  tbl_t::iterator it = tbl_.find(row);
  if (it == tbl_.end()) {
    return;
  }

  // for (row_t::const_iterator cit = it->second.begin();
  //     cit != it->second.end(); ++cit){
  // norm_ptr_->notify(row, cit->second, 0.f);
  // }

  tbl_.erase(it);
}

void sparse_matrix_storage::get_all_row_ids(vector<string>& ids) const {
  ids.clear();
  for (tbl_t::const_iterator it = tbl_.begin(); it != tbl_.end(); ++it) {
    ids.push_back(it->first);
  }
}

void sparse_matrix_storage::clear() {
  tbl_t().swap(tbl_);
  key_manager().swap(column2id_);
  // norm_ptr_->clear();
}

bool sparse_matrix_storage::save(ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << *this;
  return true;
}

bool sparse_matrix_storage::load(istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> *this;
  return true;
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
