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
#include "anchor_finder_base.hpp"
#include "recommender_builder.hpp"
#include "recommender.hpp"
#include "similarity_factory.hpp"
#include "anchor_finder_factory.hpp"
#include "anchor_builder_factory.hpp"

using namespace std;
using namespace pfi::lang;

namespace jubatus {
namespace recommender {

recommender_builder::~recommender_builder(){
}

void recommender_builder::clear_row(const std::string& id){
  originals_.erase(id);
}

void recommender_builder::update_row(const std::string& id, const sfv_diff_t& diff){
  sfvi_t& target = originals_[id];
  for (size_t i = 0; i < diff.size(); ++i){
    target.push_back(make_pair(feature2id_.get_id(diff[i].first), diff[i].second));
  }
}

void normalize(vector<pair<size_t, float> >& vals){
  float sum = 0.f;
  for (size_t i = 0; i < vals.size(); ++i){
    sum += vals[i].second;
  }
  if (sum == 0.f) return;
  for (size_t i = 0; i < vals.size(); ++i){
    vals[i].second /= sum;
  }
}

void recommender_builder::canonalize_originals(){
  pfi::data::unordered_map<std::string, sfvi_t>::iterator it = originals_.begin();
  for (; it != originals_.end(); ++it){
    sfvi_t& sfvi = it->second;
    if (sfvi.size() == 0) continue;
    
    sort(sfvi.begin(), sfvi.end());
    uint64_t prev_id = sfvi[0].first;
    float val = sfvi[0].second;
    sfvi_t new_sfvi;
    for (size_t i = 1; i < sfvi.size(); ++i){
      if (prev_id == sfvi[i].first){
        val += sfvi[i].second;
      } else {
        new_sfvi.push_back(make_pair(prev_id, val));
        prev_id = sfvi[i].first;
        val = sfvi[i].second;
      }
    }
    new_sfvi.push_back(make_pair(prev_id, val));
    sfvi.swap(new_sfvi);
  }
}

void recommender_builder::build(const recommender& base,
                                size_t all_anchor_num, 
                                size_t anchor_num_per_datum, 
                                recommender& r){
  // add previous points to originals by update_row
  pfi::data::unordered_map<string, sfv_t> previous_points;
  base.get_all_rows(previous_points);
  for (pfi::data::unordered_map<string, sfv_t>::const_iterator it = previous_points.begin(); it != previous_points.end(); ++it){
    update_row(it->first, it->second);
  }

  canonalize_originals();

  similarity_base* sim = similarity_factory::create_similarity(similarity_name_);
  if (sim == NULL){
    throw std::runtime_error("create_similarity error");
  }

  //shared_ptr<anchor_finder_base,  pfi::concurrent::threading_model::multi_thread>
  r.anchor_finder_ = 
    shared_ptr<anchor_finder_base>
    (anchor_finder_factory::create_anchor_finder(anchor_finder_name_, sim));
  if (!r.anchor_finder_){
    throw std::runtime_error("create_anchor_finder error");
  }

  anchor_builder_base* anchor_builder = anchor_builder_factory::create_anchor_builder(anchor_builder_name_);
  if (anchor_builder == NULL){
    throw std::runtime_error("create_anchor_builder error");
  }

  vector<string> anchor_ids;
  anchor_builder->build(originals_, all_anchor_num, anchor_ids);

  vector<sfvi_t> anchors;
  for (size_t i = 0; i < anchor_ids.size(); ++i){
    anchors.push_back(originals_[anchor_ids[i]]);
  }
  
  r.anchor_finder_->build_index(anchors);

  pfi::data::unordered_map<string, data2anchors_t> data2anchors;
  vector<anchor2data_t> anchor2data(anchors.size());
  for (pfi::data::unordered_map<string, sfvi_t>::const_iterator it = originals_.begin(); it != originals_.end(); ++it){
    vector<pair<size_t, float> > ret;
    r.anchor_finder_->find(it->second, anchors, anchor_num_per_datum, ret);
    //normalize(ret);
    data2anchors[it->first] = ret;
    const string& id = it->first;
    for (size_t i = 0; i < ret.size(); ++i){
      const size_t anchor_ind = ret[i].first;
      float prob = ret[i].second;
      anchor2data[anchor_ind].push_back(make_pair(id, prob));
    }
  }

  //r.anchor_finder_.swap(anchor_finder);
  r.anchors_.swap(anchors);
  r.data2anchors_.swap(data2anchors);
  r.anchor2data_.swap(anchor2data);
  r.feature2id_.swap(feature2id_);
}

void recommender_builder::get_diff(recommender_diff_t& ret) const {
  ret.clear();
  pfi::data::unordered_map<std::string, sfvi_t>::const_iterator it;
  for(it = originals_.begin(); it != originals_.end(); ++it){
    const sfvi_t& sfvi = it->second;
    sfv_t sfv;
    for (size_t i = 0; i < sfvi.size(); ++i){
      sfv.push_back(make_pair(feature2id_.get_key(sfvi[i].first), sfvi[i].second));
    }
    ret.push_back(make_pair(it->first, sfv));
  }
}

} // namespace recommender
} // namespace jubatus
