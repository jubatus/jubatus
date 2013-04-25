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

#ifndef JUBATUS_CORE_REGRESSION_PASSIVE_AGGRESSIVE_HPP_
#define JUBATUS_CORE_REGRESSION_PASSIVE_AGGRESSIVE_HPP_

#include <limits>
#include <pficommon/data/serialization.h>
#include "regression_base.hpp"

namespace jubatus {
namespace core {
namespace regression {

class passive_aggressive : public regression_base {
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

  passive_aggressive(const config& config, storage::storage_base* storage);
  explicit passive_aggressive(storage::storage_base* storage);

  void train(const sfv_t& fv, float value);

  void clear();

 private:
  config config_;
  float sum_;
  float sq_sum_;
  float count_;
};

}  // namespace regression
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_REGRESSION_PASSIVE_AGGRESSIVE_HPP_
