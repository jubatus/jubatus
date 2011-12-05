// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <algorithm>
#include "recommender_storage.hpp"

using namespace std;

namespace jubatus {
namespace storage {

recommender_storage::recommender_storage(){
}

recommender_storage::~recommender_storage(){
}

void recommender_storage::set(const std::string& row, const std::string& column, float val){
  tbl_[row][column2id_.get_id(column)] = val;
}

void recommender_storage::set_row(const std::string& row, const std::vector<std::pair<std::string, float> >& columns) {
  column_t& v = tbl_[row];
  for (size_t i = 0; i < columns.size(); ++i){
    v[column2id_.get_id(columns[i].first)] = columns[i].second;
  }
}
 
float recommender_storage::get(const std::string& row, const std::string& column) const {
  tbl_t::const_iterator it = tbl_.find(row);
  if (it == tbl_.end()){
    return 0.f;
  }

  uint64_t id = column2id_.get_id_const(column);
  if (id == key_manager::NOTFOUND){
    return 0.f;
  }

  column_t::const_iterator cit = it->second.find(id);
  if (cit == it->second.end()){
    return 0.f;
  }
  return cit->second;
}

void recommender_storage::get_row(const string& row, vector<pair<string, float> >& columns) const{
  columns.clear();
  tbl_t::const_iterator it = tbl_.find(row);
  if (it == tbl_.end()){
    return;
  }
  const column_t& v = it->second;
  for (column_t::const_iterator it = v.begin(); it != v.end(); ++it){
    columns.push_back(make_pair(column2id_.get_key(it->first), it->second));
  }
}

void recommender_storage::get_all_row_ids(std::vector<string>& ids) const{
  ids.clear();
  for (tbl_t::const_iterator it = tbl_.begin(); it != tbl_.end(); ++it){
    ids.push_back(it->first);
  }
}

void recommender_storage::clear() {
  column2id_.clear();
  tbl_.clear();
}


}
}
