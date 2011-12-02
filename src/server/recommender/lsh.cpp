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
#include "../../common/exception.hpp"

using namespace std;
using namespace pfi::data;

namespace jubatus {
namespace recommender {

static const uint64_t DEFAULT_BASE_NUM = 32; // should be in config
static const uint64_t DEFAULT_SEARCH_NUM = 16; // shoud be config

lsh::lsh() : base_num_(DEFAULT_BASE_NUM) {
  init();
}

lsh::~lsh(){
}

void lsh::init(){
  base2sorted_ids_.resize(base_num_);
  base_sq_norms_.resize(base_num_);
}

void lsh::similar_row(const sfv_t& query, vector<pair<string, float> > & ids, size_t ret_num) const{
  ids.clear();
  unordered_map<string, float> scores;
  vector<float> values;
  calc_lsh_values(query, values);
  
  for (size_t i = 0; i < values.size(); ++i){
    similar_row_using_lsh_value(values[i], base2sorted_ids_[i], scores);
  }
  
  vector<pair<float, string> > sorted_scores;
  for (unordered_map<string, float>::const_iterator it = scores.begin(); it != scores.end(); ++it){
    sorted_scores.push_back(make_pair(it->second, it->first));
  }

  sort(sorted_scores.rbegin(), sorted_scores.rend());
  for (size_t i = 0; i < sorted_scores.size() && i < ret_num; ++i){
    ids.push_back(make_pair(sorted_scores[i].second, sorted_scores[i].first));
  }
}

void lsh::clear(){
  origs_.clear();
  bases_.clear();
  base_sq_norms_.clear();
  id2base_values_.clear();
  init();
}

void lsh::clear_row(const string& id){
  throw unsupported_method("lsh::clear_row()");
}

void lsh::calc_lsh_values(const sfv_t& sfv, vector<float> values) const{
  // No values.clear() for update_row
  for (size_t i = 0; i < sfv.size(); ++i){
    const string& column = sfv[i].first;
    float val = sfv[i].second;
    unordered_map<string, vector<float> >::const_iterator it = bases_.find(column);
    if (it == bases_.end()){
      continue;
    }
    const vector<float>& v = it->second;
    // assert(v.size() == base_num_);
    for (size_t j = 0; j < v.size(); ++j){
      values[j] += v[j] * val;
    }  
  }

  for (size_t i = 0; i < values.size(); ++i){
    if (base_sq_norms_[i] != 0.f){
      values[i] /= sqrt(base_sq_norms_[i]);
    }
  }
}

void lsh::similar_row_using_lsh_value(float val, const sorted_ids_t& sorted_ids, unordered_map<string, float>& ret){
  ret.clear();
  pair<float, string> val_name(val, "");
  const sorted_ids_t::const_iterator low_it = sorted_ids.lower_bound(val_name);

  // 2 cos(x, y) = |x|^2 + |y|^2 - |x - y|^2
  // assume that |x|^2 = |y|^2 = 1 , and {ei} are orthonormal bases.
  // |x - y|^2 \approx \sum_i |ei(x) - ei(y)|^2  where ei(x) is <ei, x> / |ei|
  // 2



  // backward
  {
    sorted_ids_t::const_iterator it = low_it;
    for (size_t i = 0; i < DEFAULT_SEARCH_NUM && low_it != sorted_ids.begin(); ){
      --it;
      float dif = val - it->first;
      ret[it->second] += (2.f - dif * dif);
    }
  }

  // forward
  {
    sorted_ids_t::const_iterator it = low_it;
    for (size_t i = 0; i < DEFAULT_SEARCH_NUM && it != sorted_ids.end(); ++it){
      float dif = val - it->first;
      ret[it->second] += (2.f - dif * dif);
    }
  }
}

void lsh::generate_column_base(const string& column, vector<float>& bases){
  // should use more clever hash
  {
    unsigned int seed = 0;
    for (size_t i = 0; i < column.size(); ++i){
      seed += column[i];
      seed *= 47;
    }
    srand(seed);
  }
  bases.resize(base_num_);
  for (uint64_t i = 0; i < base_num_; ++i){
    bases[i] = (float) rand() / RAND_MAX; 
    base_sq_norms_[i] += bases[i] * bases[i];
  }
}

void lsh::update_row(const string& id, const sfv_diff_t& diff){
  origs_.set_row(id, diff);

  // delete orig_values_
  vector<float>& base_values = id2base_values_[id];
  if (base_values.size() == 0){
    base_values.resize(base_num_);
  } else {
    for (size_t i = 0; i < base_values.size(); ++i){
      sorted_ids_t::iterator it = base2sorted_ids_[i].find(make_pair(base_values[i], id));
      //assert(it != base2values_and_ids_[i].end());
      base2sorted_ids_[i].erase(it);
    }
  }

  // generate new column base
  for (size_t i = 0; i < diff.size(); ++i){
    const string& column = diff[i].first;
    if (bases_.find(column) == bases_.end()){
      vector<float> v;
      generate_column_base(column, v);
      bases_[column] = v;
    }
  }

  calc_lsh_values(diff, base_values);
  
  for (size_t i = 0; i < base_values.size(); ++i){
    base2sorted_ids_[i].insert(make_pair(base_values[i], id));
  }
}

} // namespace recommender
} // namespace jubatus

