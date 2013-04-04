// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "regression.hpp"

#include <string>
#include <utility>

#include "../common/util.hpp"
#include "../framework/mixer/mixer_factory.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../fv_converter/converter_config.hpp"
#include "../storage/storage_factory.hpp"

using std::string;
using std::pair;

using jubatus::framework::mixer::create_mixer;
using jubatus::framework::mixable_holder;
using jubatus::fv_converter::weight_manager;

namespace jubatus {
namespace core {

regression::regression(
    storage::storage_base* model_storage,
    jubatus::regression::regression_base* regression_method,
    pfi::lang::shared_ptr<framework::mixer::mixer> mixer,
    pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter)
    : mixer_(mixer),
      mixable_holder_(new mixable_holder),
      converter_(converter),
      regression_(regression_method) {
  mixable_regression_model_.set_model(
      linear_function_mixer::model_ptr(model_storage));
  wm_.set_model(mixable_weight_manager::model_ptr(new weight_manager));

  mixer_->set_mixable_holder(mixable_holder_);
  mixable_holder_->register_mixable(&mixable_regression_model_);
  mixable_holder_->register_mixable(&wm_);

  (*converter_).set_weight_manager(wm_.get_model());
}

regression::~regression() {
}

void regression::train(const pair<float, fv_converter::datum>& data) {
  sfv_t v;
  converter_->convert_and_update_weight(data.second, v);
  regression_->train(v, data.first);
}

float regression::estimate(
    const fv_converter::datum& data) const {
  sfv_t v;
  converter_->convert(data, v);
  float value = regression_->estimate(v);
  return value;
}

}  // namespace core
}  // namespace jubatus
