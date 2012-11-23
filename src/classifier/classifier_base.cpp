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

#include <float.h>
#include <queue>
#include <algorithm>
#include "classifier_base.hpp"
#include "classifier_util.hpp"
#include <assert.h>

using namespace std;


namespace jubatus{
namespace classifier {

using namespace storage;

classifier_base::classifier_base(storage::storage_base* storage) : storage_(storage), C_(1.f), use_covars_(false) {
}

classifier_base::~classifier_base(){
}

void classifier_base::classify_with_scores(const sfv_t& sfv, classify_result& scores) const{
  scores.clear();

  map_feature_val1_t ret;
  storage_->inp(sfv, ret);
  for (map_feature_val1_t::const_iterator it = ret.begin(); it != ret.end(); ++it){
    scores.push_back(classify_result_elem(it->first, it->second));
  }
}

void classifier_base::set_C(float C){
    C_ = C;
}

float classifier_base::C() const {
  return C_;
}
 
string classifier_base::classify(const sfv_t& fv) const {
  classify_result result;
  classify_with_scores(fv, result);
  float max_score = -FLT_MAX;
  string max_class;
  for (vector<classify_result_elem>::const_iterator it = result.begin(); it != result.end(); ++it){
    if (it == result.begin() || it->score > max_score){
      max_score = it->score;
      max_class = it->label;
    }
  }
  return max_class;
}

void classifier_base::update_weight(const sfv_t& sfv, float step_width, 
				    const string& pos_label, const string& neg_label){
  storage_->bulk_update(sfv, step_width, pos_label, neg_label);
}

string classifier_base::get_largest_incorrect_label(const sfv_t& fv, const string& label, classify_result& scores) const {
  classify_with_scores(fv, scores);
  float max_score = -FLT_MAX;
  string max_class;
  for (vector<classify_result_elem>::const_iterator it = scores.begin();
       it != scores.end(); ++it){
    if (it->label == label) continue;
    if (it->score > max_score || it == scores.begin()){
      max_score = it->score;
      max_class = it->label;
    }
  }
  return max_class;
}

float classifier_base::calc_margin(const sfv_t& fv, const string& label, string& incorrect_label) const{
  classify_result scores;
  incorrect_label = get_largest_incorrect_label(fv, label, scores);
  float correct_score = 0.f; 
  float incorrect_score = 0.f; 
  for (vector<classify_result_elem>::const_iterator it = scores.begin();
       it != scores.end(); ++it){
    if (it->label == label){
      correct_score = it->score;
    } else if (it->label == incorrect_label){
      incorrect_score = it->score;
    }
  }
  return incorrect_score - correct_score;
}

float classifier_base::calc_margin_and_variance(const sfv_t& sfv, const string& label, string& incorrect_label, float& var) const{
  float margin = calc_margin(sfv, label, incorrect_label);
  var = 0.f;
 
  for (size_t i = 0; i < sfv.size(); ++i){
    const string& feature = sfv[i].first;
    const float   val     = sfv[i].second; 
    feature_val2_t weight_covars;
    storage_->get2(feature, weight_covars);
    float label_covar = 1.f;
    float incorrect_label_covar = 1.f;
    for (size_t j = 0; j < weight_covars.size(); ++j){
      if (weight_covars[j].first == label){
        label_covar = weight_covars[j].second.v2;
      } else if (weight_covars[j].first == incorrect_label){
        incorrect_label_covar = weight_covars[j].second.v2;
      }
    }
    var += (label_covar + incorrect_label_covar) * val * val;
  }
  return margin;
}

float classifier_base::squared_norm(const sfv_t& fv) {
  float ret = 0.f;
  for (size_t i = 0; i < fv.size(); ++i){
    ret += fv[i].second * fv[i].second;
  }
  return ret;
}

}
}
