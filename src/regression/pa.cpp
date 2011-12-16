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
