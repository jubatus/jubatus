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

#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <stdint.h>
#include "anchor_finder_base.hpp"
#include "anchor_finder_naive.hpp"
#include "similarity_base.hpp"

#include <iostream>
using namespace std;

namespace jubatus {
namespace recommender {

class anchor_finder_naive_cmp {
public:
  bool operator()(const std::pair<size_t, float> &a, const std::pair<size_t, float> &b) const {
    return a.second < b.second;
  }
};

void anchor_finder_naive::build_index(const std::vector<sfvi_t>& anchors){
  // do nothing
}

void anchor_finder_naive::find(const sfvi_t &fv, const std::vector<sfvi_t> &anchors, size_t ret_num, data2anchors_t &ret) const {
  ret.clear();

  size_t i = 0;
  for (std::vector<sfvi_t>::const_iterator it = anchors.begin(); it != anchors.end(); ++it) {
    ret.push_back(std::make_pair(i, (*dis_)(fv, *it)));
    ++i;
  }
  std::sort(ret.rbegin(), ret.rend(), anchor_finder_naive_cmp());
  if (ret.size() > ret_num) {
    ret.resize(ret_num);
  }
}

std::string anchor_finder_naive::name() const {
  return "naive";
}

} // namespace recommender
} // namespace jubatus
