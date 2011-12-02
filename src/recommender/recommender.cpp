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
#include "recommender.hpp"
#include "anchor_finder_base.hpp"
#include "anchor_finder_factory.hpp"

using namespace std;
using namespace pfi::data;

namespace jubatus {
namespace recommender {

recommender::recommender(const recommender_data& data):
  anchor_finder_(anchor_finder_factory::unpack(data.serialized_anchor_finder,
                                               data.similarity_name, data.anchor_finder_name)),
  anchors_(data.anchors),
  anchor2data_(data.anchor2data)
{
  for(size_t i=0; i<data.data2anchors.size(); ++i){
    data2anchors_.insert(data.data2anchors[i]);
  }
  feature2id_.init_by_id2key(data.id2key);
}

recommender::recommender() {
}

recommender::~recommender(){
}

void recommender::similar_row(const string& id, vector<pair<string, float> > & ids, size_t ret_num) const{
  if (!anchor_finder_) return;
  sfv_t row;
  decode_row(id, row);
  similar_row(row, ids, ret_num);
}

void recommender::convert_from_sfv_to_sfvi(const sfv_t& sfv, sfvi_t& sfvi) const{
  sfvi.clear();
  for (size_t i = 0; i < sfv.size(); ++i){
    uint64_t id = feature2id_.get_id_const(sfv[i].first);
    if (id == key_manager::NOTFOUND){
      continue;
    }
    sfvi.push_back(make_pair(id, sfv[i].second));
  }
} 

struct id_comparator{
public:
  int operator() (const pair<string, float>& v1, 
                  const pair<string, float>& v2) const {
    if (v1.second != v2.second) return v1.second < v2.second;
    return v1.first < v2.first;
  }
};

void recommender::similar_row(const sfv_t& query, vector<pair<string, float> > & ids, size_t ret_num) const{
  if (!anchor_finder_) return;
  ids.clear();

  data2anchors_t similar_anchors;
  
  size_t anchor_num = 10;
  sfvi_t queryi;
  convert_from_sfv_to_sfvi(query, queryi);
  anchor_finder_->find(queryi, anchors_, anchor_num, similar_anchors);

  pfi::data::unordered_map<string, float> ids_map;

  for (data2anchors_t::const_iterator it = similar_anchors.begin();
       it != similar_anchors.end(); ++it) {
    size_t anchor_ind = it->first;
    anchor2data_t datum = anchor2data_[anchor_ind];
    for (anchor2data_t::const_iterator it2 = datum.begin();
         it2 != datum.end(); ++it2) {
      if (it->second > 0.0) {
	ids_map[it2->first] +=  it2->second * it->second;
      }
    }
  }

  for (pfi::data::unordered_map<string, float>::const_iterator it = ids_map.begin();
       it != ids_map.end(); ++it){
    ids.push_back(make_pair(it->first, it->second));
  }

  sort(ids.rbegin(), ids.rend(), id_comparator());
  if (ids.size() > ret_num){
    ids.resize(ret_num);
  }
}

void recommender::complete_row(const string& id, sfv_t& ret) const{
  if (!anchor_finder_) return;
  sfv_t row;
  decode_row(id, row);
  complete_row(row, ret);
}

void recommender::complete_row(const sfv_t& query, sfv_t& ret) const{
    if (!anchor_finder_) return;
  data2anchors_t similar_anchors;
  // FIXME: n should be customizable
  size_t n = 10;
  typedef unordered_map<string, pair<float, int> > sfv_map_t;
  sfv_map_t ret_map;

  sfvi_t queryi;
  convert_from_sfv_to_sfvi(query, queryi);
  anchor_finder_->find(queryi, anchors_, n, similar_anchors);

  for (data2anchors_t::const_iterator it = similar_anchors.begin();
       it != similar_anchors.end(); ++it) {
    size_t anchor_ind = it->first;
    const sfvi_t& anchor = anchors_[anchor_ind];
    sfv_t denom;

    for (sfvi_t::const_iterator it2 = anchor.begin();
         it2 != anchor.end(); ++it2) {
      const string &key = feature2id_.get_key(it2->first);
      float val = it2->second;
      pair<float, int>& v = ret_map[key];
      v.first += val;
      v.second += 1;
      /*
      if (ret_map.find(key) != ret_map.end()) {
      //      if (ret.find(key) != ret.end()) {
        ret_map[key].first += val;
        ret_map[key].second += 1.0;
      } else {
        ret_map[key].first = val;
        ret_map[key].second = 1.0;
        }*/
    }
  }

  for (sfv_map_t::iterator it = ret_map.begin(); it != ret_map.end(); ++it) {
    const string &key = it->first;
    float val = it->second.first;
    int denom = it->second.second;
    ret.push_back(make_pair(key, val/denom));
  }
}

void recommender::decode_row(const string& id, sfv_t& ret) const {
  if (!anchor_finder_) return;
  ret.clear();
  unordered_map<string, data2anchors_t>::const_iterator it = data2anchors_.find(id);
  if (it == data2anchors_.end()){
    return; 
  }

  const data2anchors_t& data2anchors = it->second;
  for (size_t i = 0; i < data2anchors.size(); ++i){
    size_t anchor_ind = data2anchors[i].first;
    float ratio = data2anchors[i].second;
    const sfvi_t& anchor = anchors_[anchor_ind];
    for (sfvi_t::const_iterator it2 = anchor.begin(); it2 != anchor.end(); ++it2){
      ret.push_back(make_pair(feature2id_.get_key(it2->first), it2->second * ratio));
    }
  }
}

void recommender::get_all_rows(unordered_map<string, sfv_t>& ret_rows) const {
  for (unordered_map<string, data2anchors_t>::const_iterator it = data2anchors_.begin(); it != data2anchors_.end(); ++it){
    sfv_t row;
    decode_row(it->first, row);
    ret_rows[it->first] = row;
  }  
} 

void recommender::swap(recommender& r){
  std::swap(anchor_finder_, r.anchor_finder_);
  std::swap(anchors_, r.anchors_);
  std::swap(data2anchors_, r.data2anchors_);
  std::swap(anchor2data_, r.anchor2data_);
  feature2id_.swap(r.feature2id_);
}

size_t recommender::anchor_num() const{
  return anchors_.size();
}

recommender_data recommender::make_recommender_data(){
  recommender_data ret;
  ret.similarity_name = anchor_finder_->similarity_name();
  ret.anchor_finder_name = anchor_finder_->name();
  stringstream ss;
  anchor_finder_factory::pack(ss, anchor_finder_->name(), anchor_finder_.get());
  ret.serialized_anchor_finder = ss.str();
  ret.anchors = anchors_;
  ret.id2key = feature2id_.get_all_id2key();
  pfi::data::unordered_map<std::string, data2anchors_t>::const_iterator it;
  for(it = data2anchors_.begin(); it != data2anchors_.end(); ++it){
    ret.data2anchors.push_back(*it);
  }
  ret.anchor2data = anchor2data_;
  return ret;
}

} // namespace recommender
} // namespace jubatus
