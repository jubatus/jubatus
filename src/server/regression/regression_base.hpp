#pragma once

#include "../../common/type.hpp"

namespace jubatus {

namespace storage {
class storage_base;
}


class regression_base {
 public:
  regression_base(storage::storage_base* storage);

  virtual ~regression_base() {}

  virtual void train(const sfv_t& fv, const float value) = 0;
  float estimate(const sfv_t& fv) const;

 protected:
  storage::storage_base* get_storage() const {
    return storage_;
  }

  void update(const sfv_t& fv, float coeff);

 private:
  storage::storage_base* storage_;
};

}
