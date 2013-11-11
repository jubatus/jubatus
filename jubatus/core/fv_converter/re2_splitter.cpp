// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "re2_splitter.hpp"

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <pficommon/lang/cast.h>
#include "exception.hpp"

using pfi::lang::lexical_cast;
using jubatus::core::fv_converter::converter_exception;

namespace jubatus {
namespace core {
namespace fv_converter {

regexp_splitter::regexp_splitter(const std::string& regexp, int group)
    : re_(regexp),
      group_(group) {
  if (group < 0) {
    throw JUBATUS_EXCEPTION(converter_exception("'group' must be positive"));
  }
  if (!re_.ok()) {
    throw JUBATUS_EXCEPTION(
        converter_exception("invalid regular expression: " + regexp));
  }
  if (group > re_.NumberOfCapturingGroups()) {
    std::string msg = "regexp '" + regexp + "' only contains "
        + lexical_cast<std::string>(re_.NumberOfCapturingGroups())
        + " groups, but 'group' is " + lexical_cast<std::string>(group);
    throw JUBATUS_EXCEPTION(converter_exception(msg));
  }
}

void regexp_splitter::split(
    const std::string& str,
    std::vector<std::pair<size_t, size_t> >& bounds) const {
  re2::StringPiece input(str.c_str());
  int groupSize = re_.NumberOfCapturingGroups() + 1;
  std::vector<re2::StringPiece> words(groupSize);
  size_t current = 0;
  while (re_.Match(input, current, input.size(), re2::RE2::UNANCHORED,
                   &(words[0]), words.size())) {
    size_t len = words[group_].length();
    size_t pos = words[group_].begin() - input.begin();
    bounds.push_back(std::make_pair(pos, len));

    size_t next = words[group_].end() - input.begin();
    if (current == next) {
      // did not match
      current += 1;
    } else {
      current = next;
    }
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
