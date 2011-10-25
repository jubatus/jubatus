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

#include "ux_splitter.hpp"

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "../../server/fv_converter/util.hpp"
#include "../../server/fv_converter/exception.hpp"

using namespace std;

namespace jubatus {

ux_splitter::ux_splitter(const vector<string>& keywords) {
  vector<string> keys(keywords);
  trie_.clear();
  trie_.build(keys, true);
}

ux_splitter::~ux_splitter() {
}

void ux_splitter::split(const string& string,
                        vector<pair<size_t, size_t> >& ret_boundaries) {
  vector<pair<size_t, size_t> > bounds;
  for (size_t i = 0; i < string.size(); ++i) {
    size_t len = 0;
    ux::id_t id = trie_.prefixSearch(string.c_str() + i, string.size() - i, len);
    if (id == ux::NOTFOUND) {
      continue;
    }
    bounds.push_back(make_pair(i, len));
    i += len - 1;
  }

  bounds.swap(ret_boundaries);
}

static void read_all_lines(const char* file_name, vector<string>& lines) {
  ifstream ifs(file_name);
  if (!ifs) {
    throw converter_exception(string("cannot open: ") + file_name);
  }
  for (string line; getline(ifs, line); ) {
    lines.push_back(line);
  }
}

}

extern "C" {

jubatus::ux_splitter*
create(const map<string, string>& params) {
  const string& path = jubatus::get_or_die(params, "dict_path");
  vector<string> lines;
  jubatus::read_all_lines(path.c_str(), lines);
  
  return new jubatus::ux_splitter(lines);
}

}
