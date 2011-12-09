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

#include "bit_index_storage.hpp"

using namespace std;
using namespace pfi::data;

namespace jubatus {
namespace storage{

bit_index_storage::bit_index_storage(){
}
bit_index_storage::~bit_index_storage(){
}

void bit_index_storage::set_row(const string& row, const bit_vector& bv){
  row2bitvals_[row] = bv;
}

void bit_index_storage::remove_row(const string& row){
  row2bitvals_.erase(row);
}

void bit_index_storage::clear(){
  row2bitvals_.clear();
}

void bit_index_storage::get_diff(sparse_matrix_storage& sms) const{
  
}

void bit_index_storage::set_mixed_and_clear_diff(sparse_matrix_storage& mixed) const{
}

void bit_index_storage::similar_row(const bit_vector& bv, vector<pair<string, float> >& ids, uint64_t ret_num) const {
  ids.clear();
  uint64_t bit_num = bv.bit_num();
  if (bit_num == 0){
    return;
  }
  vector<pair<uint64_t, string> > scores;
  for (unordered_map<string, bit_vector>::const_iterator it = row2bitvals_.begin();
       it != row2bitvals_.end(); ++it){
    uint64_t match_num = bv.calc_hamming_similarity(it->second);
    if (scores.size() < ret_num){
      scores.push_back(make_pair(match_num, it->first));
    } else if (scores.size() == ret_num){
      make_heap(scores.begin(), scores.end());
    } else {
      if (match_num <= scores.front().first) continue;
      pop_heap(scores.begin(), scores.end());
      scores.back() = make_pair(match_num, it->first);
      push_heap(scores.begin(), scores.end());
    }
  }

  sort(scores.rbegin(), scores.rend());
  for (size_t i = 0; i < scores.size() && i < ret_num; ++i){
    ids.push_back(make_pair(scores[i].second, (float)scores[i].first / bit_num));
  }
}

bool bit_index_storage::save(std::ostream& os){
  pfi::data::serialization::binary_oarchive oa(os);
  oa << *this;
  return true;
}

bool bit_index_storage::load(std::istream& is){
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> *this;
  return true;
}

string bit_index_storage::name() const{
  return string("bit_index_storage");
}

}
}
