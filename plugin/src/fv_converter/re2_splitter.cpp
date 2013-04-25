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

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <pficommon/lang/cast.h>
#include "jubatus/core/fv_converter/exception.hpp"
#include "re2_splitter.hpp"

using pfi::lang::lexical_cast;
using jubatus::core::fv_converter::converter_exception;

namespace jubatus {
namespace plugin {
namespace fv_converter {

re2_splitter::re2_splitter(const std::string& regexp, int group)
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

void re2_splitter::split(
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
}  // namespace plugin
}  // namespace jubatus

static const std::string& get(
    const std::map<std::string, std::string>& args,
    const std::string& key) {
  std::map<std::string, std::string>::const_iterator it = args.find(key);
  if (it == args.end()) {
    throw JUBATUS_EXCEPTION(converter_exception("not found: " + key));
  } else {
    return it->second;
  }
}

static int get_int_with_default(
    const std::map<std::string, std::string>& args,
    const std::string& key,
    int default_value) {
  if (args.count(key) == 0) {
    return default_value;
  }
  std::string s = get(args, key);
  try {
    return pfi::lang::lexical_cast<int>(s);
  } catch (const std::bad_cast&) {
    throw JUBATUS_EXCEPTION(
        converter_exception("is not integer: " + key + " = " + s));
  }
}

extern "C" {
jubatus::plugin::fv_converter::re2_splitter* create(
    const std::map<std::string, std::string>& args) {
  std::string pattern = get(args, "pattern");
  int group = get_int_with_default(args, "group", 0);
  return new jubatus::plugin::fv_converter::re2_splitter(pattern, group);
}
}
