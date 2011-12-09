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

#include "inverted_index_storage.hpp"

using namespace std;
using namespace pfi::data;

namespace jubatus {
namespace storage {

inverted_index_storage::inverted_index_storage(){
}

inverted_index_storage::~inverted_index_storage(){
}

void inverted_index_storage::set(const std::string& row, const std::string& column, float val){
  inv_diff_[row][column2id_.get_id(column)] = val;
}
 

void inverted_index_storage::remove(const std::string& row, const std::string& column){
  inv_diff_[row][column2id_.get_id(row)] = 0.f;
}

void inverted_index_storage::clear(){
  inv_.clear();
  inv_diff_.clear();
}

void inverted_index_storage::get_diff(sparse_matrix_storage& diff) const{
  diff.clear();
  for (tbl_t::const_iterator it = inv_diff_.begin(); it != inv_diff_.end(); ++it){
    vector<pair<string, float> > columns;
    for (row_t::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2){
      columns.push_back(make_pair(column2id_.get_key(it2->first), it2->second));
    }
    diff.set_row(it->first, columns);
  }
}

void inverted_index_storage::set_mixed_and_clear_diff(const sparse_matrix_storage& mixed_diff) {
  vector<string> ids;
  mixed_diff.get_all_row_ids(ids);
  for (size_t i = 0; i < ids.size(); ++i){
    const string& row = ids[i];
    row_t& v = inv_[row];
    vector<pair<string, float> > columns;
    mixed_diff.get_row(row, columns);
    for (size_t j = 0; j < columns.size(); ++j){
      if (columns[j].second == 0.f){
        v.erase(column2id_.get_id(columns[j].first));
      } else {
        v[column2id_.get_id(columns[j].first)] = columns[j].second;
      }
    }
  }
  inv_diff_.clear();
}

void inverted_index_storage::mix(const sparse_matrix_storage& diff){
  vector<string> ids;
  diff.get_all_row_ids(ids);
  for (size_t i = 0; i < ids.size(); ++i){
    const string& row = ids[i];
    row_t& v = inv_[row];
    vector<pair<string, float> > columns;
    diff.get_row(row, columns);
    for (size_t j = 0; j < columns.size(); ++j){
      v[column2id_.get_id(columns[j].first)] = columns[j].second;
    }
  }
}

bool inverted_index_storage::save(std::ostream& os){ 
  pfi::data::serialization::binary_oarchive oa(os);
  oa << *this;
  return true;
}

bool inverted_index_storage::load(std::istream& is){
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> *this;
  return true;
}

void inverted_index_storage::calc_scores(const sfv_t& query, pfi::data::unordered_map<std::string, float>& scores) const {
  pfi::data::unordered_map<uint64_t, float> i_scores;
  for (size_t i = 0; i < query.size(); ++i){
    const string& fid = query[i].first;
    float val = query[i].second;
    sfv_t column;
    add_inp_scores(fid, val, i_scores);
  }

  for (pfi::data::unordered_map<uint64_t, float>::const_iterator it = i_scores.begin();
       it != i_scores.end(); ++it){
    scores[column2id_.get_key(it->first)] += it->second;
  }
}

void inverted_index_storage::add_inp_scores(const std::string& row, 
                                            float val, 
                                            pfi::data::unordered_map<uint64_t, float>& scores) const{
  pfi::data::unordered_map<uint64_t, float> i_scores;
  tbl_t::const_iterator it_diff = inv_diff_.find(row);
  if (it_diff != inv_diff_.end()){
    const row_t& row_v = it_diff->second;
    for (row_t::const_iterator row_it = row_v.begin(); row_it != row_v.end(); ++row_it){
      i_scores[row_it->first] = row_it->second * val;
    }
  }

  tbl_t::const_iterator it = inv_.find(row);
  if (it != inv_.end()){
    const row_t& row_v = it->second;
    for (row_t::const_iterator row_it = row_v.begin(); row_it != row_v.end(); ++row_it){
      if (i_scores.find(row_it->first) == i_scores.end()){
        i_scores[row_it->first] = row_it->second * val;
      }
    }
  }

  for (pfi::data::unordered_map<uint64_t, float>::const_iterator it = i_scores.begin();
       it != i_scores.end(); ++it){
    scores[it->first] += it->second;
  }

  
}


std::string inverted_index_storage::name() const{
  return string("inverted_index_storage");
}



}
}
