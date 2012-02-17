// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include "pa.hpp"

#include <cfloat>
#include <cmath>
#include <iostream>

namespace jubatus {
namespace regression {

using namespace std;

PA::PA(storage::storage_base* storage)
    : regression_base(storage), epsilon_(0.1), C_(FLT_MAX),
      sum_(0), sq_sum_(0), count_(0) {
}

static float calc_norm(const sfv_t& fv) {
  float norm = 0;
  for (size_t i = 0; i < fv.size(); ++i) {
    norm += fv[i].second * fv[i].second;
  }
  return norm;
}

void PA::train(const sfv_t& fv, float value) {
  sum_ += value;
  sq_sum_ += value * value;
  count_ += 1;
  float avg = sum_ / count_;
  float std_dev = sqrt(sq_sum_ / count_
                       - 2 * avg * sum_ / count_
                       + avg * avg);
  float fv_norm = sqrt(calc_norm(fv));

  float predict = estimate(fv);
  float error = value - predict;
  float sign_error = error > 0 ? 1.0f : -1.0f;
  float loss = sign_error * error - epsilon_ * std_dev;

  if (loss > 0) {
    float coeff = sign_error * std::min(C_, loss) / (fv_norm * fv_norm);
    if (!isinf(coeff)) {
      update(fv, coeff);
    }
  }
}

}
}
