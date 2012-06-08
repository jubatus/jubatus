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

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "word_splitter.hpp"

using namespace std;
using namespace jubatus::fv_converter;

class my_splitter : public word_splitter {
 public:
  void split(const std::string& str,
             std::vector<std::pair<size_t, size_t> >& bounds) {
    size_t p = 0;
    while (true) {
      size_t b = str.find_first_not_of(' ', p);
      if (b == string::npos)
        break;
      size_t e = str.find_first_of(' ', b);
      if (e == string::npos)
        e = str.size();
      bounds.push_back(make_pair(b, e - b));
      p = e;
    }
  }
};

extern "C" {

word_splitter* create(const map<string, string>& params) {
  return new my_splitter();
}

}

