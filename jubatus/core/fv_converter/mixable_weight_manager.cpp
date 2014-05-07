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

#include "../fv_converter/weight_manager.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

versioned_weight_diff mixable_weight_manager::get_diff_impl() const {
  return get_model()->get_diff();
}

bool mixable_weight_manager::put_diff_impl(
    const versioned_weight_diff& diff) {
  return get_model()->put_diff(diff);
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
}


}  // namespace driver
}  // namespace core
}  // namespace jubatus
