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

#include <vector>
#include <iostream>
#include "re2_filter.hpp"
#include "exception.hpp"

using namespace std;
using namespace re2;

namespace jubatus {
namespace fv_converter {

re2_filter::re2_filter(const string& regexp, const string& replace) 
    : re_(regexp), replace_(replace) {
  if (!re_.ok()) {
    throw JUBATUS_EXCEPTION(converter_exception("invalid regular expression: " + regexp));
  }
  
}

void re2_filter::filter(const string& input, string& output) const {
  output = input;
  RE2::GlobalReplace(&output, re_, replace_);
}

}
}

