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

using std::sort;
using std::string;

void sort_and_merge(sfv_t& sfv) {
  if (sfv.size() == 0) {
    return;
  }
  sort(sfv.begin(), sfv.end());
  sfv_t ret_sfv;
  const string* prev = &sfv[0].first;
  float val = sfv[0].second;
  for (size_t i = 1; i < sfv.size(); ++i) {
    if (sfv[i].first == *prev) {
      val += sfv[i].second;
    } else {
      ret_sfv.push_back(make_pair(*prev, val));
      prev = &sfv[i].first;
      val = sfv[i].second;
    }
  }
  ret_sfv.push_back(make_pair(*prev, val));
  sfv.swap(ret_sfv);
}

}  // namespace jubatus
