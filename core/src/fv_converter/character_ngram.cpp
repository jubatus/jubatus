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

#include "character_ngram.hpp"

#include <string>
#include <utility>
#include <vector>

namespace jubatus {
namespace core {
namespace fv_converter {

namespace {

bool is_begin_of_character(unsigned char c) {
  return (c & 0xC0) != 0x80;
}

}  // namespace

void character_ngram::split(
    const std::string& string,
    std::vector<std::pair<size_t, size_t> >& ret_boundaries) const {
  const size_t len = length_;
  std::vector<size_t> queue(len);
  size_t p = 0;
  size_t n = 0;

  std::vector<std::pair<size_t, size_t> > bounds;
  for (size_t i = 1; i <= string.size(); ++i) {
    if (i == string.size() || is_begin_of_character(string[i])) {
      ++n;
      if (n >= len) {
        size_t b = queue[p];
        bounds.push_back(std::make_pair(b, i - b));
      }
      queue[p] = i;
      ++p;
      if (p == len) {
        p = 0;
      }
    }
  }

  bounds.swap(ret_boundaries);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
