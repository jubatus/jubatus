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
#include "recommender_base.hpp"
#include "../common/vector_util.hpp"

using namespace std;
using namespace pfi::data;

namespace jubatus {
namespace recommender {

const uint64_t recommender_base::complete_row_similar_num = 7;

recommender_base::recommender_base(){
}

recommender_base::~recommender_base(){
}

void recommender_base::similar_row(const std::string& id, std::vector<std::pair<std::string, float> > & ids, size_t ret_num) const{
  ids.clear();
  sfv_t sfv;
  origs_.get_row(id, sfv);
  similar_row(sfv, ids, ret_num);
}

void recommender_base::decode_row(const std::string& id, sfv_t& ret) const{
  ret.clear();
  origs_.get_row(id, ret);
}

void recommender_base::get_all_row_ids(std::vector<std::string>& ids) const{
  origs_.get_all_row_ids(ids);
}

void recommender_base::complete_row(const std::string& id, sfv_t& ret) const{
  ret.clear();
  sfv_t sfv;
  origs_.get_row(id, sfv);
  complete_row(sfv, ret);
}

void recommender_base::complete_row(const sfv_t& query, sfv_t& ret) const{
  ret.clear();
  vector<pair<string, float> > ids;
  similar_row(query, ids, complete_row_similar_num);
  if (ids.size() == 0) return;
  for (size_t i = 0; i < ids.size(); ++i){
    sfv_t row;
    origs_.get_row(ids[i].first, row);
    sort_and_merge(row);
    float ratio = ids[i].second;
    for (size_t j = 0; j < row.size(); ++j){
      ret.push_back(make_pair(row[j].first, row[j].second * ratio));
    }
  }
  sort_and_merge(ret);
  for (size_t i = 0; i < ret.size(); ++i){
    ret[i].second /= ids.size();
  }
}

}
}
