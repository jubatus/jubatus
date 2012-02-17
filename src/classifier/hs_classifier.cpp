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

#include <fstream>
#include "hs_classifier.hpp"

using namespace std;

namespace hs{

HSClassifier::HSClassifier(){
}

HSClassifier::~HSClassifier(){
}

void HSClassifier::Clear(){
  feature2id_.clear();
  class2id_.clear();
  vector<string>().swap(id2class_);
  vector<vector<float> >().swap(weights_);
}

void HSClassifier::SaveKey2ID(const key2id_t& key2id,
			      ofstream& ofs) const{
  uint64_t key_num = key2id.size();
  ofs.write((const char*)& key_num, sizeof(key_num) * 1);

  for (key2id_t::const_iterator it = key2id.begin();
       it != key2id.end(); ++it){
    const string& key = it->first;
    uint64_t len = key.size();
    ofs.write((const char*) &len, sizeof(len) * 1);
    ofs.write((const char*) &key[0], sizeof(key[0]) * len);
    uint64_t val = it->second;
    ofs.write((const char*) &val, sizeof(val));
  }
}

void HSClassifier::LoadKey2ID(key2id_t& key2id,
			      ifstream& ifs) {
  uint64_t key_num = 0;
  ifs.read((char*)&key_num, sizeof(key_num) * 1);

  for (uint64_t i = 0; i < key_num; ++i){
    uint64_t len = 0;
    ifs.read((char*) &len, sizeof(len) * 1);
    string key;
    key.resize(len);
    ifs.read((char*) &key[0], sizeof(key[0]) * len);
    uint64_t val = 0;
    ifs.read((char*) &val, sizeof(val));
    key2id[key] = val;
  }
}


int HSClassifier::Save(const string& filename) const{
  ofstream ofs(filename.c_str());
  if (!ofs){
    return -1;
  }
  SaveKey2ID(feature2id_, ofs);
  SaveKey2ID(class2id_, ofs);

  for (size_t i = 0; i < weights_.size(); ++i){
    const vector<float>& v = weights_[i];
    ofs.write((const char*)&v[0], sizeof(v[0]) * v.size());
  }

  if (!ofs){
    return -1;
  }

  return 0;
}

int HSClassifier::Load(const string& filename) {
  Clear();
  ifstream ifs(filename.c_str());
  if (!ifs){
    return -1;
  }
  LoadKey2ID(feature2id_, ifs);
  LoadKey2ID(class2id_, ifs);

  id2class_.resize(class2id_.size());
  for (key2id_t::const_iterator it = class2id_.begin(); it != class2id_.end(); ++it){
    id2class_[it->second] = it->first;
  } 
  weights_.resize(class2id_.size(), vector<float>(feature2id_.size()));

  if (!ifs) return -1;
  
  return 0;
}

void HSClassifier::Train(const key_str_t& input,
			 const string& output){
  vector<uint64_t> features;
  for (key_str_t::const_iterator it = input.begin(); it != input.end(); ++it){
    ExtractFeatureConst(it->first, it->second, features);
  }
  uint64_t target_class =  GetClassID(output);
  vector<float> scores;
  CalcScores(features, scores);
  uint64_t max_score_class = GetMaxScoreID(scores);
  if (target_class == max_score_class) {
    return; // no update
  } 
  
  features.clear();
  for (key_str_t::const_iterator it = input.begin(); it != input.end(); ++it){
    ExtractFeature(it->first, it->second, features);
  }
  Update(features, target_class, max_score_class);
}

void HSClassifier::Update(const std::vector<uint64_t>& features,
			  const uint64_t target_class, const uint64_t max_score_class) {
  for (size_t i = 0; i < features.size(); ++i){
    uint64_t id = features[i];
    weights_[target_class][id] += 1;
    weights_[max_score_class][id] -= 1;
  }
}

uint64_t HSClassifier::GetMaxScoreID(const vector<float>& scores) const {
  if (scores.size() == 0) return 0;
  float max_score = scores[0];
  uint64_t max_id = 0;
  for (size_t i = 1; i < scores.size(); ++i){
    if (scores[i] > max_score) {
      max_score = scores[i];
      max_id    = i;
    }
  }
  return max_id;
}

void HSClassifier::CalcScores(const vector<uint64_t>& features, vector<float>& scores) const{
  size_t class_num = id2class_.size();
  scores.resize(class_num);
  fill(scores.begin(), scores.end(), 0.f);
  for (size_t i = 0; i < features.size(); ++i){
    uint64_t id = features[i];
    for (size_t j = 0; j < class_num; ++j){
      scores[j] += weights_[j][id];
    }
  }
}  

void HSClassifier::ExtractFeatureConst(const std::string& field, const std::string& value,
				       vector<uint64_t>& features) const{
  uint64_t total_id = GetIDConst(field + "/" + value);
  if (total_id != NOTFOUND){
    features.push_back(total_id);
  }
  
  // UTF-8 bigram feature
  string cur;
  string prev;
  bool first = true;
  for (size_t i = 0; ; ++i){
    if (first ||
	(i != value.size() && (value[i] & 0xC0) == 0x80)){
      cur += value[i];
      first = false;
      continue;
    }
    uint64_t term_id = GetIDConst(field + "/" + prev + cur);
    if (term_id != NOTFOUND){
      features.push_back(term_id);
    }
    if (i == value.size()) break;
    prev = cur;
    cur = value[i];
  }
}

void HSClassifier::ExtractFeature(const std::string& field, const std::string& value,
				  vector<uint64_t>& features) {
  features.push_back(GetID(field + "/" + value));
  
  // UTF-8 bigram feature
  string cur;
  string prev;
  bool first = true;
  for (size_t i = 0; ; ++i){
    if (first ||
	(i != value.size() && (value[i] & 0xC0) == 0x80)){
      cur += value[i];
      first = false;
      continue;
    }
    features.push_back(GetID(field + "/" + prev + cur));

    if (i == value.size()) break;
    prev = cur;
    cur = value[i];
  }
}



key_double_t HSClassifier::Classify(const key_str_t& input) const {
  vector<uint64_t> features;
  for (key_str_t::const_iterator it = input.begin(); it != input.end(); ++it){
    ExtractFeatureConst(it->first, it->second, features);
  }
  vector<float> scores;
  CalcScores(features, scores);

  key_double_t class2score;
  for (size_t i = 0; i < scores.size(); ++i){
    class2score[id2class_[i]] = scores[i];
  }
  return class2score;
}

uint64_t HSClassifier::GetID(const string& key){
  key2id_t::const_iterator it = feature2id_.find(key);
  if (it != feature2id_.end()){
    return it->second;
  }
  uint64_t new_id = static_cast<uint64_t>(feature2id_.size());
  feature2id_[key] = new_id;
  for (size_t i = 0; i < weights_.size(); ++i){
    weights_[i].resize(new_id+1);
  }
  return new_id;
}

uint64_t HSClassifier::GetIDConst(const string& key) const{
  key2id_t::const_iterator it = feature2id_.find(key);
  if (it != feature2id_.end()){
    return it->second;
  } else {
    return NOTFOUND;
  }
}

uint64_t HSClassifier::GetClassID(const string& output){
  key2id_t::const_iterator it = class2id_.find(output);
  if (it != class2id_.end()){
    return it->second;
  }
  uint64_t new_id = static_cast<uint64_t>(class2id_.size());
  class2id_[output] = new_id;
  id2class_.push_back(output);

  weights_.resize(new_id+1);
  weights_[new_id].resize(feature2id_.size());
  return new_id;
}

}
