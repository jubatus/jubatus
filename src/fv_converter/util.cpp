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

#include "util.hpp"
#include "exception.hpp"
#include <pficommon/lang/cast.h>

using namespace std;

namespace jubatus {

const string& get_or_die(const map<string, string> & params,
                         const string& key) {
  map<string, string>::const_iterator it = params.find(key);
  if (it == params.end()) {
    throw converter_exception(string("\"" + key +  "\" is not specified in parameters"));
  }
  return it->second;
}

std::string get_with_default(const std::map<std::string, std::string> & params,
                             const std::string& key,
                             const std::string& default_value) {
  map<string, string>::const_iterator it = params.find(key);
  if (it == params.end()) {
    return default_value;
  } else {
    return it->second;
  }
}

const int get_int_or_die(const map<string, string>& params,
                         const string& key) {
  const string& s = get_or_die(params, key);
  try {
    return pfi::lang::lexical_cast<int>(s);
  } catch (const bad_cast& e) {
    throw converter_exception(string("\"" + key + "\" must be an integer value: " + s));
  }
}

}
