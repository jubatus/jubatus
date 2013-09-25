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

#include "windowset_config.hpp"

namespace jubatus {
namespace fv_converter {
namespace window {

std::vector<window_process>
string_config_convert(const std::vector<std::string>& conf) {
  std::vector<window_process> result;
  result.reserve(conf.size());
  for (std::vector<std::string>::const_iterator it = conf.begin();
       it != conf.end();
       ++it) {
    if (*it == "sum") {
      result.push_back(SUM);
    } else if (*it == "avg") {
      result.push_back(AVG);
    } else if (*it == "var") {
      result.push_back(VAR);
    } else if (*it == "min") {
      result.push_back(MIN);
    } else if (*it == "max") {
      result.push_back(MAX);
    } else {
      throw JUBATUS_EXCEPTION(unsupported_method(*it));
    }
  }
  return result;
}

}  // namespace window
}  // namespace fv_converter
}  // namespace jubatus


