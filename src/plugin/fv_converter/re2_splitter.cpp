// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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
#include <iostream>
#include "../../fv_converter/exception.hpp"
#include "re2_splitter.hpp"
#include <map>

using namespace std;
using pfi::lang::lexical_cast;
using jubatus::fv_converter::converter_exception;

namespace jubatus {

re2_splitter::re2_splitter(const string& regexp, int group)
    : re_(regexp), group_(group) {
  if (group < 0) {
    throw converter_exception("'group' must be positive");
  }
  if (!re_.ok()) {
    throw converter_exception("invalid regular expression: " + regexp);
  }
  if (group > re_.NumberOfCapturingGroups()) {
    string msg = "regexp '" + regexp + "' only contains "
        + lexical_cast<string>(re_.NumberOfCapturingGroups())
        + " groups, but 'group' is "
        + lexical_cast<string>(group);
    throw converter_exception(msg);
  }
}

void re2_splitter::split(const string& str,
                         vector<pair<size_t, size_t> >& bounds) {
  re2::StringPiece input(str.c_str());
  int groupSize = re_.NumberOfCapturingGroups() + 1;
  vector<re2::StringPiece> words(groupSize);
  size_t current = 0;
  while (re_.Match(input, current, input.size(), re2::RE2::UNANCHORED,
                   &(words[0]), words.size())) {
    size_t len = words[group_].length();
    size_t pos = words[group_].begin() - input.begin();
    bounds.push_back(make_pair(pos, len));

    size_t next = words[group_].end() - input.begin();
    if (current == next) {
      // did not match
      current += 1;
    } else {
      current = next;
    }
  }
}

}

static
const string& get(const map<string, string>& args, const string& key) {
  map<string, string>::const_iterator it = args.find(key);
  if (it == args.end()) {
    throw converter_exception("not found: " + key);
  } else {
    return it->second;
  }
}

static
int get_int_with_default(const map<string, string>& args,
                         const string& key, int default_value) {
  if (args.count(key) == 0) {
    return default_value;
  }
  string s = get(args, key);
  try {
    return pfi::lang::lexical_cast<int>(s);
  } catch(const bad_cast&) {
    throw converter_exception("is not integer: " + key + " = " + s);
  }
}

extern "C" {
  jubatus::re2_splitter* create(const map<string, string>& args) {
    string pattern = get(args, "pattern");
    int group = get_int_with_default(args, "group", 0);
    return new jubatus::re2_splitter(pattern, group);
  }
}
