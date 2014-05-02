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

#ifndef JUBATUS_CORE_REGRESSION_REGRESSION_BASE_HPP_
#define JUBATUS_CORE_REGRESSION_REGRESSION_BASE_HPP_

#include <map>
#include <string>
#include "jubatus/util/lang/shared_ptr.h"
#include "../common/type.hpp"
#include "../framework/linear_function_mixer.hpp"

namespace jubatus {
namespace core {

namespace storage {
class storage_base;
}  // namespace storage

namespace regression {

class regression_base {
 public:
  explicit regression_base(storage_ptr storage);

  virtual ~regression_base() {
  }

  virtual void train(const common::sfv_t& fv, const float value) = 0;
  float estimate(const common::sfv_t& fv) const;

  virtual void clear();

  // TODO(beam2d): Think the objective of this function and where it should be
  // defined. Algorithms have |get_status| tentatively to extract status from
  // storages.
  virtual void get_status(std::map<std::string, std::string>& status) const;

  storage_ptr get_storage();

 protected:
  void update(const common::sfv_t& fv, float coeff);

  storage_ptr storage_;

};

}  // namespace regression

}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_REGRESSION_REGRESSION_BASE_HPP_
