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

#include "feature_hasher.hpp"

#include <string>
#include <pficommon/data/functional_hash.h>
#include <pficommon/lang/cast.h>
#include "../common/exception.hpp"
#include "../common/hash.hpp"
#include "exception.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

feature_hasher::feature_hasher(uint64_t max)
    : max_size_(max) {
  if (max == 0) {
    throw JUBATUS_EXCEPTION(
        converter_exception("feature max size must be positive"));
  }
}

void feature_hasher::hash_feature_keys(sfv_t& fv) const {
  for (size_t i = 0, size = fv.size(); i < size; ++i) {
    uint64_t id = hash_util::calc_string_hash(fv[i].first) % max_size_;
    fv[i].first = pfi::lang::lexical_cast<std::string>(id);
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
