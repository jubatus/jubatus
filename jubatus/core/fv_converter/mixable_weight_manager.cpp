// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "mixable_weight_manager.hpp"

#include "jubatus/util/data/serialization.h"

#include "../fv_converter/weight_manager.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

using fv_converter::keyword_weights;

versioned_weight_diff::versioned_weight_diff() {
}

versioned_weight_diff::versioned_weight_diff(const keyword_weights& w)
  : weights_(w) {
}

versioned_weight_diff::versioned_weight_diff(const keyword_weights& w,
                                             const storage::version& v)
  : weights_(w), version_(v) {
}

versioned_weight_diff& versioned_weight_diff::merge(const versioned_weight_diff& target) {
  if (version_ == target.version_) {
    weights_.merge(target.weights_);
  } else if (version_ < target.version_) {
    weights_ = target.weights_;
    version_ = target.version_;
  }
  return *this;
}

versioned_weight_diff mixable_weight_manager::get_diff_impl() const {
  return versioned_weight_diff(get_model()->get_diff(), version_);
}

bool mixable_weight_manager::put_diff_impl(
    const versioned_weight_diff& diff) {
  if (diff.version_ == version_) {
    get_model()->put_diff(diff.weights_);
    version_.increment();
    return true;
  } else {
    return false;
  }
}

void mixable_weight_manager::mix_impl(
    const versioned_weight_diff& lhs,
    const versioned_weight_diff& rhs,
    versioned_weight_diff& acc) const {
  if (lhs.version_ == rhs.version_) {
    acc = rhs;
    acc.weights_.merge(lhs.weights_);
  } else if (lhs.version_ < rhs.version_) {
    acc = rhs;
  } else {
    acc = lhs;
  }
}

void mixable_weight_manager::clear() {
  get_model()->clear();
  version_.reset();
}

storage::version mixable_weight_manager::get_version() const {
  return version_;
}


}  // namespace driver
}  // namespace core
}  // namespace jubatus
