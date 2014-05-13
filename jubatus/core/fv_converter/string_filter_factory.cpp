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

#include <string>
#include "exception.hpp"
#include "regexp_filter.hpp"
#include "string_filter.hpp"
#include "util.hpp"

using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace fv_converter {

namespace {

shared_ptr<regexp_filter> create_regexp_filter(
    const param_t& params) {
  const std::string& pattern = get_or_die(params, "pattern");
  const std::string& replace = get_or_die(params, "replace");
  return shared_ptr<regexp_filter>(new regexp_filter(pattern, replace));
}

}  // namespace

shared_ptr<string_filter> string_filter_factory::create(
    const std::string& name,
    const param_t& params) const {
  string_filter* p;
  if (name == "regexp") {
    return create_regexp_filter(params);
  } else if (ext_ && (p = ext_(name, params))) {
    return shared_ptr<string_filter>(p);
  } else {
    throw JUBATUS_EXCEPTION(
        converter_exception("unknown filter name: " + name));
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
