// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <pficommon/lang/cast.h>

#include "num_filter_factory.hpp"
#include "num_filter_impl.hpp"
#include "exception.hpp"

#include "util.hpp"
#include "dynamic_num_filter.hpp"

using namespace std;
using namespace jubatus::fv_converter;

namespace jubatus {

static
add_filter* create_add_filter(const map<string, string>& params) {
  const string& value = get_or_die(params, "value");
  double float_val = pfi::lang::lexical_cast<double>(value);
  return new add_filter(float_val);
}

static
num_filter* create_dynamic_filter(const map<string, string>& params) {
  const string& path = get_or_die(params, "path");
  const string& function = get_or_die(params, "function");
  return new dynamic_num_filter(path, function, params);
}

num_filter* num_filter_factory::create(
    const string& name,
    const map<string, string>& params) const {
  if (name == "add") {
    return create_add_filter(params);
  } else if (name == "dynamic") {
    return create_dynamic_filter(params);
  } else {
    throw JUBATUS_EXCEPTION(converter_exception("unknonw num filter name: " + name));
  }
}

}
