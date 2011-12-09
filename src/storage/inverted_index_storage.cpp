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
  inv_.set(row, column, val);
}
 
void inverted_index_storage::get_row(const std::string& row, std::vector<std::pair<std::string, float> >& columns) const{
  inv_.get_row(row, columns);
}

void inverted_index_storage::remove(const std::string& row, const std::string& column){
  inv_.remove(row, column);
}

void inverted_index_storage::clear(){
  inv_.clear();
}

void inverted_index_storage::get_diff(sparse_matrix_storage& sms) const{
}

void inverted_index_storage::set_mixed_and_clear_diff(sparse_matrix_storage& mixed) const{
}

std::string inverted_index_storage::name() const{
  return string("inverted_index_storage");
}

}
}
