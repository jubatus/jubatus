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

#include "passive_aggressive.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace jubatus {
namespace core {
namespace regression {

passive_aggressive::passive_aggressive(
    const config& config,
    storage::storage_base* storage)
    : regression_base(storage),
      config_(config),
      sum_(0.f),
      sq_sum_(0.f),
      count_(0.f) {
}

passive_aggressive::passive_aggressive(storage::storage_base* storage)
    : regression_base(storage),
      sum_(0.f),
      sq_sum_(0.f),
      count_(0.f) {
}

static float squared_norm(const sfv_t& fv) {
  float norm = 0.f;
  for (size_t i = 0; i < fv.size(); ++i) {
    norm += fv[i].second * fv[i].second;
  }
  return norm;
}

void passive_aggressive::train(const sfv_t& fv, float value) {
  sum_ += value;
  sq_sum_ += value * value;
  count_ += 1;
  float avg = sum_ / count_;
  float std_dev = sqrt(sq_sum_ / count_ -  avg * avg);

  float predict = estimate(fv);
  float error = value - predict;
  float sign_error = error > 0.f ? 1.0f : -1.0f;
  float loss = sign_error * error - config_.epsilon * std_dev;

  if (loss > 0.f) {
    float coeff = sign_error * std::min(config_.C, loss) / squared_norm(fv);
    if (!std::isinf(coeff)) {
      update(fv, coeff);
    }
  }
}

void passive_aggressive::clear() {
  regression_base::clear();
  sum_ = 0.f;
  sq_sum_ = 0.f;
  count_ = 0.f;
}

}  // namespace regression
}  // namespace core
}  // namespace jubatus
