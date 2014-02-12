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

#ifndef JUBATUS_CORE_FV_CONVERTER_MIXABLE_WEIGHT_MANAGER_HPP_
#define JUBATUS_CORE_FV_CONVERTER_MIXABLE_WEIGHT_MANAGER_HPP_

#include "../framework/mixable.hpp"
#include "weight_manager.hpp"
#include "../common/version.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

struct versioned_weight_diff {
  versioned_weight_diff();
  explicit versioned_weight_diff(const fv_converter::keyword_weights& w);
  versioned_weight_diff(const fv_converter::keyword_weights& w,
                        const storage::version& v);
  versioned_weight_diff& merge(const versioned_weight_diff& target);

  MSGPACK_DEFINE(weights_, version_);

  fv_converter::keyword_weights weights_;
  storage::version version_;
};

class mixable_weight_manager : public framework::mixable<
    fv_converter::weight_manager,
    fv_converter::versioned_weight_diff> {
 public:
  fv_converter::versioned_weight_diff get_diff_impl() const;

  bool put_diff_impl(const versioned_weight_diff& diff);

  void mix_impl(
      const versioned_weight_diff& lhs,
      const versioned_weight_diff& rhs,
      versioned_weight_diff& acc) const;
  void clear();

  storage::version get_version() const;

 private:
  storage::version version_;
};

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FV_CONVERTER_MIXABLE_WEIGHT_MANAGER_HPP_
