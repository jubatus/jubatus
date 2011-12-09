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
  inv_diff_.set(row, column, val);
}
 
void inverted_index_storage::get_row(const std::string& row, std::vector<std::pair<std::string, float> >& columns) const{
  inv_diff_.get_row(row, columns);
  inv_.get_row_with_filter(row, columns);
}

void inverted_index_storage::remove(const std::string& row, const std::string& column){
  inv_diff_.set(row, column, 0.f);
}

void inverted_index_storage::clear(){
  inv_.clear();
  inv_diff_.clear();
}

void inverted_index_storage::get_diff(sparse_matrix_storage& diff) const{
  diff = inv_diff_;
}

void inverted_index_storage::set_mixed_and_clear_diff(const sparse_matrix_storage& mixed_diff) {
  vector<string> ids;
  mixed_diff.get_all_row_ids(ids);
  for (size_t i = 0; i < ids.size(); ++i){
    const string& row = ids[i];
    vector<pair<string, float> > columns;
    get_row(row, columns);
    for (size_t j = 0; j < columns.size(); ++j){
      if (columns[j].second == 0.f){
        inv_.remove(row, columns[j].first);
      } else {
        inv_.set(row, columns[j].first, columns[j].second);
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
    vector<pair<string, float> > columns;
    get_row(row, columns);
    for (size_t j = 0; j < columns.size(); ++j){
      inv_.set(row, columns[j].first, columns[j].second);
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


std::string inverted_index_storage::name() const{
  return string("inverted_index_storage");
}



}
}
