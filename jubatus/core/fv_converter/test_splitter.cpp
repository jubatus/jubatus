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

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "word_splitter.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

class my_splitter : public word_splitter {
 public:
  void split(
      const std::string& str,
      std::vector<std::pair<size_t, size_t> >& bounds) const {
    size_t p = 0;
    while (true) {
      size_t b = str.find_first_not_of(' ', p);
      if (b == std::string::npos) {
        break;
      }
      size_t e = str.find_first_of(' ', b);
      if (e == std::string::npos) {
        e = str.size();
      }
      bounds.push_back(std::make_pair(b, e - b));
      p = e;
    }
  }
};

extern "C" {
word_splitter* create(const std::map<std::string, std::string>& params) {
  return new my_splitter();
}
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
