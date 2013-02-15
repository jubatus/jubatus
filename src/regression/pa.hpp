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

#ifndef JUBATUS_REGRESSION_PA_HPP_
#define JUBATUS_REGRESSION_PA_HPP_

#include <limits>
#include <pficommon/data/serialization.h>
#include "regression_base.hpp"

namespace jubatus {
namespace regression {

class PA : public regression_base {
 public:
  struct config {
    config()
        : C(std::numeric_limits<float>::max()),
          epsilon(0.1f) {
    }
    float C;
    float epsilon;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & NAMED_MEMBER("regularization_weight", C)
          & NAMED_MEMBER("sensitivity", epsilon);
    }
  };

  PA(const config& config, storage::storage_base* storage);
  explicit PA(storage::storage_base* storage);

  void train(const sfv_t& fv, float value);

 private:
  config config_;
  float sum_;
  float sq_sum_;
  float count_;
};

}  // namespace regression
}  // namespace jubatus

#endif  // JUBATUS_REGRESSION_PA_HPP_
