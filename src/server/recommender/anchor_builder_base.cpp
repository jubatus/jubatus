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

#include "anchor_builder_base.hpp"

using namespace std;

namespace jubatus {
namespace recommender {

void anchor_builder_base::build(const pfi::data::unordered_map<string, sfvi_t> originals, 
                                size_t anchor_num,
                                vector<string>& anchors){
  anchors.clear();
  vector<string> ids;
  vector<sfvi_t> sfvis;
  for (pfi::data::unordered_map<string, sfvi_t>::const_iterator it = originals.begin(); it != originals.end(); ++it){
    ids.push_back(it->first);
    sfvis.push_back(it->second);
  }
  vector<size_t> anchor_inds;
  build(sfvis, anchor_num, anchor_inds);
  for (size_t i = 0; i < anchor_inds.size(); ++i){
    anchors.push_back(ids[anchor_inds[i]]);
  }
}


} // namespace recommender
} // namespace jubatus
