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

#include <string>
#include <algorithm>

#include "jubatus/util/lang/bind.h"
#include "linear_function_mixer.hpp"

using std::string;
using jubatus::util::lang::bind;
using jubatus::util::lang::_1;
using jubatus::util::lang::_2;

using jubatus::core::storage::val3_t;
using jubatus::core::storage::feature_val3_t;
using jubatus::core::storage::features3_t;

namespace jubatus {
namespace core {
namespace driver {

namespace {

val3_t mix_val3(double w1, double w2, const val3_t& lhs, const val3_t& rhs) {
  return val3_t(
      (w1 * lhs.v1 + w2 * rhs.v1) / (w1 + w2),
      std::min(lhs.v2, rhs.v2),
      (w1 * lhs.v3 + w2 * rhs.v3) / (w1 + w2));
}

feature_val3_t mix_feature(
    double w1,
    double w2,
    const feature_val3_t& lhs,
    const feature_val3_t& rhs) {
  val3_t def(0, 1, 0);
  feature_val3_t ret(lhs);
  storage::detail::binop(ret, rhs, bind(mix_val3, w1, w2, _1, _2), def);
  return ret;
}

}  // namespace

void linear_function_mixer::mix_impl(
    const diffv& lhs,
    const diffv& rhs,
    diffv& mixed) const {
  if (lhs.v.expect_version == rhs.v.expect_version) {
    features3_t l(lhs.v.diff);
    const features3_t& r(rhs.v.diff);
    storage::detail::binop(
        l,
        r,
        bind(mix_feature, lhs.count, rhs.count, _1, _2));
    mixed.v.diff.swap(l);
    mixed.count = lhs.count + rhs.count;
    mixed.v.expect_version = lhs.v.expect_version;
  } else if (lhs.v.expect_version < rhs.v.expect_version) {
    mixed = rhs;
  } else {
    mixed = lhs;
  }
}

diffv linear_function_mixer::get_diff_impl() const {
  diffv ret;
  ret.count = 1;  // TODO(kuenishi) mixer_->get_count();
  get_model()->get_diff(ret.v);
  return ret;
}

bool linear_function_mixer::put_diff_impl(const diffv& v) {
  return get_model()->set_average_and_clear_diff(v.v);
}

storage::version linear_function_mixer::get_version() const {
  return get_model()->get_version();
}

void linear_function_mixer::clear() {
  get_model()->clear();
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
