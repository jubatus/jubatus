// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_FV_CONVERTER_FEATURE_HASHER_HPP_
#define JUBATUS_CORE_FV_CONVERTER_FEATURE_HASHER_HPP_

#include <stdint.h>
#include "../common/type.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

class feature_hasher {
 public:
  explicit feature_hasher(uint64_t max);

  void hash_feature_keys(sfv_t& fv) const;

 private:
  uint64_t max_size_;
};

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FV_CONVERTER_FEATURE_HASHER_HPP_
