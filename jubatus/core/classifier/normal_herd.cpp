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

#include "normal_herd.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "classifier_util.hpp"

using std::string;

namespace jubatus {
namespace core {
namespace classifier {

normal_herd::normal_herd(classifier_base::storage_ptr storage)
    : classifier_base(storage) {
  config_.C = 0.1f;
}

normal_herd::normal_herd(
    const classifier_config& config,
    classifier_base::storage_ptr storage)
    : classifier_base(storage),
      config_(config) {
}

void normal_herd::train(const common::sfv_t& sfv, const string& label) {
  string incorrect_label;
  float variance = 0.f;
  float margin = -calc_margin_and_variance(sfv, label, incorrect_label,
                                           variance);
  if (margin >= 1.f) {
    get_storage()->register_label(label);
    return;
  }
  update(sfv, margin, variance, label, incorrect_label);
}

void normal_herd::update(
    const common::sfv_t& sfv,
    float margin,
    float variance,
    const string& pos_label,
    const string& neg_label) {
  storage::storage_base* sto = get_storage();
  for (common::sfv_t::const_iterator it = sfv.begin(); it != sfv.end(); ++it) {
    const string& feature = it->first;
    float val = it->second;
    storage::feature_val2_t ret;
    sto->get2(feature, ret);

    storage::val2_t pos_val(0.f, 1.f);
    storage::val2_t neg_val(0.f, 1.f);
    ClassifierUtil::get_two(ret, pos_label, neg_label, pos_val, neg_val);

    float val_covariance_pos = val * pos_val.v2;
    float val_covariance_neg = val * neg_val.v2;

    const float C = config_.C;
    sto->set2(
        feature,
        pos_label,
        storage::val2_t(
            pos_val.v1
                + (1.f - margin) * val_covariance_pos
                    / (val_covariance_pos * val + 1.f / C),
            1.f
                / ((1.f / pos_val.v2) + (2 * C + C * C * variance)
                    * val * val)));
    if (neg_label != "") {
      sto->set2(
          feature,
          neg_label,
          storage::val2_t(
              neg_val.v1
                  - (1.f - margin) * val_covariance_neg
                      / (val_covariance_neg * val + 1.f / C),
              1.f
                  / ((1.f / neg_val.v2) + (2 * C + C * C * variance)
                      * val * val)));
    }
  }
  touch(pos_label);
}

std::string normal_herd::name() const {
  return string("normal_herd");
}

}  // namespace classifier
}  // namespace core
}  // namespace jubatus
