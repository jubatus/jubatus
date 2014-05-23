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

#include "classifier_base.hpp"

#include <assert.h>
#include <float.h>

#include <algorithm>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include "jubatus/util/lang/bind.h"

#include "../common/exception.hpp"
#include "classifier_util.hpp"

using std::string;
using std::vector;
using jubatus::core::storage::map_feature_val1_t;
using jubatus::core::storage::feature_val2_t;

namespace jubatus {
namespace core {
namespace classifier {

classifier_base::classifier_base(storage_ptr storage)
  : storage_(storage) {
}

classifier_base::~classifier_base() {
}

void classifier_base::set_label_unlearner(
    jubatus::util::lang::shared_ptr<unlearner::unlearner_base>
        label_unlearner) {
  label_unlearner->set_callback(
      jubatus::util::lang::bind(
          &classifier_base::delete_label, this, jubatus::util::lang::_1));
}

void classifier_base::classify_with_scores(
    const common::sfv_t& sfv,
    classify_result& scores) const {
  scores.clear();

  map_feature_val1_t ret;
  storage_->inp(sfv, ret);
  for (map_feature_val1_t::const_iterator it = ret.begin(); it != ret.end();
      ++it) {
    scores.push_back(classify_result_elem(it->first, it->second));
  }
}

string classifier_base::classify(const common::sfv_t& fv) const {
  classify_result result;
  classify_with_scores(fv, result);
  float max_score = -FLT_MAX;
  string max_class;
  for (vector<classify_result_elem>::const_iterator it = result.begin();
      it != result.end(); ++it) {
    if (it == result.begin() || it->score > max_score) {
      max_score = it->score;
      max_class = it->label;
    }
  }
  return max_class;
}

void classifier_base::clear() {
  storage_->clear();
}

vector<string> classifier_base::get_labels() const {
  return storage_->get_labels();
}

bool classifier_base::set_label(const string& label) {
  return storage_->set_label(label);
}

void classifier_base::get_status(std::map<string, string>& status) const {
  storage_->get_status(status);
  status["storage"] = storage_->type();
}

void classifier_base::update_weight(
    const common::sfv_t& sfv,
    float step_width,
    const string& pos_label,
    const string& neg_label) {
  storage_->bulk_update(sfv, step_width, pos_label, neg_label);
}

string classifier_base::get_largest_incorrect_label(
    const common::sfv_t& fv,
    const string& label,
    classify_result& scores) const {
  classify_with_scores(fv, scores);
  float max_score = -FLT_MAX;
  string max_class;
  for (vector<classify_result_elem>::const_iterator it = scores.begin();
      it != scores.end(); ++it) {
    if (it->label == label) {
      continue;
    }
    if (it->score > max_score || it == scores.begin()) {
      max_score = it->score;
      max_class = it->label;
    }
  }
  return max_class;
}

float classifier_base::calc_margin(
    const common::sfv_t& fv,
    const string& label,
    string& incorrect_label) const {
  classify_result scores;
  incorrect_label = get_largest_incorrect_label(fv, label, scores);
  float correct_score = 0.f;
  float incorrect_score = 0.f;
  for (vector<classify_result_elem>::const_iterator it = scores.begin();
      it != scores.end(); ++it) {
    if (it->label == label) {
      correct_score = it->score;
    } else if (it->label == incorrect_label) {
      incorrect_score = it->score;
    }
  }
  return incorrect_score - correct_score;
}

float classifier_base::calc_margin_and_variance(
    const common::sfv_t& sfv,
    const string& label,
    string& incorrect_label,
    float& var) const {
  float margin = calc_margin(sfv, label, incorrect_label);
  var = 0.f;

  for (size_t i = 0; i < sfv.size(); ++i) {
    const string& feature = sfv[i].first;
    const float val = sfv[i].second;
    feature_val2_t weight_covars;
    storage_->get2(feature, weight_covars);
    float label_covar = 1.f;
    float incorrect_label_covar = 1.f;
    for (size_t j = 0; j < weight_covars.size(); ++j) {
      if (weight_covars[j].first == label) {
        label_covar = weight_covars[j].second.v2;
      } else if (weight_covars[j].first == incorrect_label) {
        incorrect_label_covar = weight_covars[j].second.v2;
      }
    }
    var += (label_covar + incorrect_label_covar) * val * val;
  }
  return margin;
}

float classifier_base::squared_norm(const common::sfv_t& fv) {
  float ret = 0.f;
  for (size_t i = 0; i < fv.size(); ++i) {
    ret += fv[i].second * fv[i].second;
  }
  return ret;
}

storage_ptr classifier_base::get_storage() {
  return storage_;
}

void classifier_base::touch(const std::string& label) {
  if (unlearner_) {
    unlearner_->touch(label);
  }
}

bool classifier_base::delete_label(const std::string& label) {
  return get_storage()->delete_label(label);
}

}  // namespace classifier
}  // namespace core
}  // namespace jubatus
