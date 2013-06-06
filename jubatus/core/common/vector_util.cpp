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

#include "vector_util.hpp"
#include <algorithm>
#include <string>

namespace jubatus {
namespace core {
namespace common {

using std::sort;
using std::string;

void sort_and_merge(sfv_t& sfv) {
  if (sfv.size() <= 1) {
    return;
  }
  sort(sfv.begin(), sfv.end());

  typedef sfv_t::iterator iterator;
  iterator cur = sfv.begin();
  iterator end = sfv.end();
  for (iterator iter = cur+1; iter != end; ++iter) {
    if (iter->first == cur->first) {
      cur->second += iter->second;
    } else {
      ++cur;
      *cur = *iter;
    }
  }
  sfv.erase(cur+1, end);
}

}  // namespace common
}  // namespace core
}  // namespace jubatus
