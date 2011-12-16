#pragma once

#include "regression_base.hpp"

namespace jubatus {
namespace regression {

class PA : public regression_base {
 public:
  PA(storage::storage_base* storage);

  void train(const sfv_t& fv, float value);

 private:
  float epsilon_;
  float C_;
  float sum_;
  float sq_sum_;
  float count_;
};

}
}
