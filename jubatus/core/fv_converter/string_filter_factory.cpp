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

#include "string_filter_factory.hpp"

#include <map>
#include <string>
#include "dynamic_string_filter.hpp"
#include "exception.hpp"
#include "regexp_filter.hpp"
#include "string_filter.hpp"
#include "util.hpp"

using pfi::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace fv_converter {

namespace {

shared_ptr<regexp_filter> create_regexp_filter(
    const string_filter_factory::param_t& params) {
  const std::string& pattern = get_or_die(params, "pattern");
  const std::string& replace = get_or_die(params, "replace");
  return shared_ptr<regexp_filter>(new regexp_filter(pattern, replace));
}

shared_ptr<string_filter> create_dynamic_filter(
    const string_filter_factory::param_t& params) {
  const std::string& path = get_or_die(params, "path");
  const std::string& function = get_or_die(params, "function");
  return shared_ptr<string_filter>(
      new dynamic_string_filter(path, function, params));
}

}  // namespace

shared_ptr<string_filter> string_filter_factory::create(
    const std::string& name,
    const std::map<std::string, std::string>& params) const {
  if (name == "regexp") {
    return create_regexp_filter(params);
  } else if (name == "dynamic") {
    return create_dynamic_filter(params);
  } else {
    throw JUBATUS_EXCEPTION(
        converter_exception("unknown filter name: " + name));
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
