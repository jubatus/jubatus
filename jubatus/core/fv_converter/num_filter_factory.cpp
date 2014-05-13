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

#include <map>
#include <string>
#include "jubatus/util/lang/cast.h"
#include "exception.hpp"
#include "num_filter_factory.hpp"
#include "num_filter_impl.hpp"
#include "util.hpp"

using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace fv_converter {

namespace {

shared_ptr<add_filter> create_add_filter(
    const std::map<std::string, std::string>& params) {
  const std::string& value = get_or_die(params, "value");
  double float_val = jubatus::util::lang::lexical_cast<double>(value);
  return shared_ptr<add_filter>(new add_filter(float_val));
}

}  // namespace

shared_ptr<num_filter> num_filter_factory::create(
    const std::string& name,
    const param_t& params) const {
  num_filter* p;
  if (name == "add") {
    return create_add_filter(params);
  } else if (ext_ && (p = ext_(name, params))) {
    return shared_ptr<num_filter>(p);
  } else {
    throw JUBATUS_EXCEPTION(
        converter_exception("unknonw num filter name: " + name));
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
