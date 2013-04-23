// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "num_feature_factory.hpp"

#include <string>
#include "exception.hpp"
#include "dynamic_num_feature.hpp"
#include "util.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

num_feature* create_dynamic_num_feature(
    const num_feature_factory::param_t& params) {
  const std::string& path = get_or_die(params, "path");
  const std::string& function = get_or_die(params, "function");
  return new dynamic_num_feature(path, function, params);
}

num_feature* num_feature_factory::create(
    const std::string& name,
    const num_feature_factory::param_t& params) const {
  if (name == "dynamic") {
    return create_dynamic_num_feature(params);
  } else {
    throw JUBATUS_EXCEPTION(
        converter_exception(std::string("unknonwn num feature name: ") + name));
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
