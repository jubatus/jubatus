// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "string_filter_factory.hpp"
#include "string_filter.hpp"
#include "exception.hpp"

#include "util.hpp"
#ifdef HAVE_RE2
#  include "re2_filter.hpp"
#endif
#include "dynamic_string_filter.hpp"

using namespace std;

namespace jubatus {

#ifdef HAVE_RE2
static
re2_filter* create_re2_filter(const string_filter_factory::param_t& params) {
  const string& pattern = get_or_die(params, "pattern");
  const string& replace = get_or_die(params, "replace");
  return new re2_filter(pattern, replace);
}
#endif

static
string_filter* create_dynamic_filter(const string_filter_factory::param_t& params) {
  const string& path = get_or_die(params, "path");
  const string& function = get_or_die(params, "function");
  return new dynamic_string_filter(path, function, params);
}

string_filter* string_filter_factory::create(const string& name,
                                             const map<string, string>& params) const {
#ifdef HAVE_RE2
  if (name == "regexp") {
    return create_re2_filter(params);
  } else
#endif
  if (name == "dynamic") {
    return create_dynamic_filter(params);
  } else {
    throw converter_exception("unknown filter name: " + name);
  }
}

}
