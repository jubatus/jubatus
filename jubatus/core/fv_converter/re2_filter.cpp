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

#include <string>
#include <vector>
#include "re2_filter.hpp"
#include "exception.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {
re2_filter::re2_filter(const std::string& regexp, const std::string& replace)
    : re_(regexp),
      replace_(replace) {
  if (!re_.ok()) {
    throw JUBATUS_EXCEPTION(
        converter_exception("invalid regular expression: " + regexp));
  }
}

void re2_filter::filter(const std::string& input, std::string& output) const {
  output = input;
  RE2::GlobalReplace(&output, re_, replace_);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
