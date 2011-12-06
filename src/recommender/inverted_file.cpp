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
#include "inverted_file.hpp"
#include "../common/exception.hpp"
#include "../common/vector_util.hpp"

using namespace std;
using namespace pfi::data;

namespace jubatus {
namespace recommender {


inverted_file::inverted_file(){
}

inverted_file::~inverted_file(){
}

void inverted_file::similar_row(const sfv_t& query, std::vector<std::pair<std::string, float> > & ids, size_t ret_num) const{
  ids.clear();
  pfi::data::unordered_map<string, float> scores;
  for (size_t i = 0; i < query.size(); ++i){
    const string& fid = query[i].first;
    float val = query[i].second;
    sfv_t column;
    invs_.get_row(fid, column);
    sort_and_merge(column);
    for (size_t j = 0; j < column.size(); ++j){
      scores[column[j].first] += column[j].second * val;
    }
  }
  
  vector<pair<float, string> > sorted_scores;
  for (pfi::data::unordered_map<string, float>::const_iterator it = scores.begin(); it != scores.end(); ++it){
    sorted_scores.push_back(make_pair(it->second, it->first));
  }
  sort(sorted_scores.rbegin(), sorted_scores.rend());
  for (size_t i = 0; i < sorted_scores.size() && i < ret_num; ++i){
    ids.push_back(make_pair(sorted_scores[i].second, sorted_scores[i].first));
  }
}


void inverted_file::clear(){
  origs_.clear();
  invs_.clear();
}

void inverted_file::clear_row(const std::string& id){
  throw unsupported_method("inverted_file::clear_row()"); // not supported yet.
}

void inverted_file::update_row(const std::string& id, const sfv_diff_t& diff){
  origs_.set_row(id, diff);

  for (size_t i = 0; i < diff.size(); ++i){
    invs_.set(diff[i].first, id, diff[i].second);
  }
}

} // namespace recommender
} // namespace jubatus
