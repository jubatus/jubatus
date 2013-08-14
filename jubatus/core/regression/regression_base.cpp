// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "regression_base.hpp"
#include "../storage/storage_base.hpp"

namespace jubatus {
namespace core {
namespace regression {

regression_base::regression_base(storage_ptr storage)
    : mixable_(new driver::linear_function_mixer) {
  mixable_->set_model(storage);
}

float regression_base::estimate(const common::sfv_t& fv) const {
  storage::map_feature_val1_t ret;
  get_storage()->inp(fv, ret);
  return ret["+"];
}

void regression_base::update(const common::sfv_t& fv, float coeff) {
  get_storage()->bulk_update(fv, coeff, "+", "");
}

void regression_base::clear() {
  get_storage()->clear();
}

void regression_base::register_mixables(framework::mixable_holder& holder)
    const {
  holder.register_mixable(mixable_.get());
}

}  // namespace regression
}  // namespace core
}  // namespace jubatus
