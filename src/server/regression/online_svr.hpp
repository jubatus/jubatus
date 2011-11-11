#pragma once

#include "regression_base.hpp"

namespace jubatus {

class online_svr : public regression_base {
 public:
  online_svr(storage::storage_base* storage);

  void train(const sfv_t& fv, float value);

 private:
  float epsilon_;
  float C_;
};

}
