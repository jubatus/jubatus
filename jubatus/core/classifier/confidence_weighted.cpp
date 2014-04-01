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

#include "confidence_weighted.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "classifier_util.hpp"

using std::string;

namespace jubatus {
namespace core {
namespace classifier {

confidence_weighted::confidence_weighted(classifier_base::storage_ptr storage)
    : classifier_base(storage) {
}

confidence_weighted::confidence_weighted(
    const classifier_config& config,
    classifier_base::storage_ptr storage)
    : classifier_base(storage),
      config_(config) {
}

void confidence_weighted::train(const common::sfv_t& sfv, const string& label) {
  const float C = config_.C;
  string incorrect_label;
  float variance = 0.f;
  float margin = -calc_margin_and_variance(sfv, label, incorrect_label,
                                           variance);
  float b = 1.f + 2 * C * margin;
  float gamma = -b + std::sqrt(b * b - 8 * C * (margin - C * variance));

  if (gamma <= 0.f) {
    get_storage()->register_label(label);
    return;
  }
  gamma /= 4 * C * variance;
  update(sfv, gamma, label, incorrect_label);
}

void confidence_weighted::update(
    const common::sfv_t& sfv,
    float step_width,
    const string& pos_label,
    const string& neg_label) {
  storage::storage_base* sto = get_storage();
  for (common::sfv_t::const_iterator it = sfv.begin(); it != sfv.end(); ++it) {
    const string& feature = it->first;
    float val = it->second;
    storage::feature_val2_t val2;
    sto->get2(feature, val2);

    storage::val2_t pos_val(0.f, 1.f);
    storage::val2_t neg_val(0.f, 1.f);
    ClassifierUtil::get_two(val2, pos_label, neg_label, pos_val, neg_val);

    const float C = config_.C;
    float covar_pos_step = 2.f * step_width * val * val * C;
    float covar_neg_step = 2.f * step_width * val * val * C;

    sto->set2(
        feature,
        pos_label,
        storage::val2_t(pos_val.v1 + step_width * pos_val.v2 * val,
                        1.f / (1.f / pos_val.v2 + covar_pos_step)));
    if (neg_label != "") {
      sto->set2(
          feature,
          neg_label,
          storage::val2_t(neg_val.v1 - step_width * neg_val.v2 * val,
                          1.f / (1.f / neg_val.v2 + covar_neg_step)));
    }
  }
  touch(pos_label);
}

string confidence_weighted::name() const {
  return string("confidence_weighted");
}

}  // namespace classifier
}  // namespace core
}  // namespase jubatus
