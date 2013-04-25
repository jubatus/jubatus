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

#include <pficommon/data/serialization.h>

#include "../fv_converter/weight_manager.hpp"

using jubatus::core::fv_converter::keyword_weights;
using jubatus::core::fv_converter::weight_manager;

namespace jubatus {
namespace core {
namespace driver {

keyword_weights mixable_weight_manager::get_diff_impl() const {
  return get_model()->get_diff();
}

void mixable_weight_manager::put_diff_impl(
    const fv_converter::keyword_weights& diff) {
  get_model()->put_diff(diff);
}

void mixable_weight_manager::mix_impl(
    const keyword_weights& lhs,
    const keyword_weights& rhs,
    keyword_weights& acc) const {
  acc = rhs;
  acc.merge(lhs);
}

void mixable_weight_manager::clear() {
  get_model()->clear();
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
