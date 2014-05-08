// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "binary_feature_factory.hpp"

#include <map>
#include <string>
#include "exception.hpp"
#include "util.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

binary_feature* binary_feature_factory::create(
    const std::string& name,
    const param_t& params) const {
  binary_feature* p;
  if (ext_ != NULL && (p = ext_(name, params))) {
    return p;
  } else {
    throw JUBATUS_EXCEPTION(
        converter_exception("unknown binary feature name: " + name));
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
