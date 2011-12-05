#include "online_svr.hpp"

#include <cfloat>
#include <cmath>

namespace jubatus {

using namespace std;

online_svr::online_svr(storage::storage_base* storage)
    : regression_base(storage), epsilon_(0.0), C_(FLT_MAX) {
}

static float calc_norm(const sfv_t& fv) {
  float norm = 0;
  for (size_t i = 0; i < fv.size(); ++i) {
    norm += fv[i].second * fv[i].second;
  }
  return norm;
}

void online_svr::train(const sfv_t& fv, float value) {
  float predict = estimate(fv);
  float error = value - predict;
  float sign_error = error > 0 ? 1.0f : -1.0f;
  float loss = sign_error * error - epsilon_;

  if (loss > 0) {
    float fv_norm = calc_norm(fv);
    float coeff = sign_error * std::min(C_, loss / fv_norm);
    if (!isinf(coeff)) {
      update(fv, coeff);
    }
  }
}

}
