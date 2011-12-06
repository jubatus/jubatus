#include "regression_base.hpp"
#include "../storage/storage_base.hpp"

namespace jubatus {

regression_base::regression_base(storage::storage_base* storage)
      : storage_(storage) {}

float regression_base::estimate(const sfv_t& fv) const {
  storage::map_feature_val1_t ret;
  get_storage()->inp(fv, ret);
  return ret["+"];
}

void regression_base::update(const sfv_t& fv, float coeff) {
  storage_->bulk_update(fv, coeff, "+", "");
}


}
