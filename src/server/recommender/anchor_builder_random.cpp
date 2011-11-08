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

#include <algorithm>

#include "anchor_builder_random.hpp"

using namespace std;

namespace jubatus {
namespace recommender {

void anchor_builder_random::build(const vector<sfvi_t>& sfvs,
                                  size_t anchor_num,
                                  vector<size_t>& anchors){
  anchors.clear();
  size_t choosed_num = min(anchor_num, sfvs.size());
  for (size_t i = 0; i < sfvs.size(); ++i){
    anchors.push_back(i);
  }
  random_shuffle(anchors.begin(), anchors.end());
  anchors.resize(choosed_num);
}

} // namespace recommender
} // namespace jubatus

