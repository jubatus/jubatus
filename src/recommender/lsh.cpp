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
#include <cmath>
#include "lsh.hpp"
#include "../common/exception.hpp"
#include "../common/hash.hpp"

using namespace std;
using namespace pfi::data;
using namespace pfi::lang;

namespace jubatus {
namespace recommender {

static const uint64_t DEFAULT_BASE_NUM = 64; // should be in config

lsh::lsh(shared_ptr<storage::recommender_storage> storage, uint64_t base_num)
  : recommender_base(storage),base_num_(base_num) {
}

lsh::lsh(shared_ptr<storage::recommender_storage> storage)
  : recommender_base(storage), base_num_(DEFAULT_BASE_NUM) {
}

lsh::~lsh(){
}

void lsh::similar_row(const sfv_t& query, vector<pair<string, float> > & ids, size_t ret_num) const{
  ids.clear();
  if (base_num_ == 0) return;

  bit_vector query_bv;
  calc_lsh_values(query, query_bv);
  
  vector<pair<uint64_t, string> > scores;
  for (unordered_map<string, bit_vector>::const_iterator it = row2lshvals_.begin();
       it != row2lshvals_.end(); ++it){
    uint64_t match_num = query_bv.calc_hamming_similarity(it->second);
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
    ids.push_back(make_pair(scores[i].second, (float)scores[i].first / base_num_));
  }
}

void lsh::clear(){
  origs_->clear();
  column2baseval_.clear();
  row2lshvals_.clear();
}

void lsh::clear_row(const string& id){
  origs_->remove_row(id);
  row2lshvals_.erase(id);
}

void lsh::calc_lsh_values(const sfv_t& sfv, bit_vector& bv) const{
  // No values.clear() for update_row
  vector<float> lsh_vals(base_num_);
  for (size_t i = 0; i < sfv.size(); ++i){
    const string& column = sfv[i].first;
    float val = sfv[i].second;
    unordered_map<string, vector<float> >::const_iterator it = column2baseval_.find(column);
    if (it == column2baseval_.end()){
      continue;
    }
    const vector<float>& v = it->second;
    // assert(v.size() == base_num_);
    for (size_t j = 0; j < v.size(); ++j){
      lsh_vals[j] += v[j] * val;
    }  
  }

  bv.resize_and_clear(base_num_);
  for (size_t i = 0; i < lsh_vals.size(); ++i){
    if (lsh_vals[i] >= 0.f){
      bv.set_bit(i);
    }
  }
}

void lsh::generate_column_base(const string& column){
  // should use more clever hash
  srand(hash_util::calc_string_hash(column));
  vector<float>& baseval = column2baseval_[column];
  baseval.resize(base_num_);
  for (uint64_t i = 0; i < base_num_; ++i){
    float v1 = (float)(rand()+1) / ((float)RAND_MAX + 1);
    float v2 = (float)(rand()+1) / ((float)RAND_MAX + 1);
    float z = sqrt(-2.f * log(v1)) * cos(2.f * M_PI * v2);
    baseval[i] = z;
  }
}

void lsh::update_row(const string& id, const sfv_diff_t& diff){
  origs_->set_row(id, diff);
  bit_vector& bv = row2lshvals_[id];
  sfv_t row;
  origs_->get_row(id, row);
  calc_lsh_values(row, bv);
}

} // namespace recommender
} // namespace jubatus

