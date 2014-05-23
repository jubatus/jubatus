// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "so_factory.hpp"
#include <string>
#include "jubatus/core/fv_converter/util.hpp"

#include "dynamic_string_filter.hpp"
#include "dynamic_binary_feature.hpp"
#include "dynamic_num_feature.hpp"
#include "dynamic_num_filter.hpp"
#include "dynamic_string_feature.hpp"

using jubatus::core::fv_converter::binary_feature;
using jubatus::core::fv_converter::num_filter;
using jubatus::core::fv_converter::num_feature;
using jubatus::core::fv_converter::string_feature;
using jubatus::core::fv_converter::string_filter;
using jubatus::core::fv_converter::param_t;
using jubatus::core::fv_converter::get_or_die;

namespace jubatus {
namespace server {
namespace fv_converter {

binary_feature* so_factory::create_binary_feature(
    const std::string& name,
    const param_t& params) const {
  if (name == "dynamic") {
    const std::string& path = get_or_die(params, "path");
    const std::string& function = get_or_die(params, "function");
    return new dynamic_binary_feature(path, function, params);
  }
  return NULL;
}


num_filter* so_factory::create_num_filter(
    const std::string& name,
    const param_t& params) const {
  if (name == "dynamic") {
    const std::string& path = get_or_die(params, "path");
    const std::string& function = get_or_die(params, "function");
    return new dynamic_num_filter(path, function, params);
  }
  return NULL;
}

num_feature* so_factory::create_num_feature(
    const std::string& name,
    const param_t& params) const {
  if (name == "dynamic") {
    const std::string& path = get_or_die(params, "path");
    const std::string& function = get_or_die(params, "function");
    return new dynamic_num_feature(path, function, params);
  }
  return NULL;
}

string_feature* so_factory::create_string_feature(
    const std::string& name,
    const param_t& params) const {
  if (name == "dynamic") {
    const std::string& path = get_or_die(params, "path");
    const std::string& function = get_or_die(params, "function");
    return new dynamic_string_feature(path, function, params);
  }
  return NULL;
}

string_filter* so_factory::create_string_filter(
    const std::string& name,
    const param_t& params) const {
  if (name == "dynamic") {
    const std::string& path = get_or_die(params, "path");
    const std::string& function = get_or_die(params, "function");
    return new dynamic_string_filter(path, function, params);
  }
  return NULL;
}

}  // namespace fv_converter
}  // namespace server
}  // namespace jubatus
