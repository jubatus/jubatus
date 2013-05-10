// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "key_manager.hpp"
#include <algorithm>
#include <string>
#include <vector>

using std::string;
using std::vector;
using pfi::data::unordered_map;

namespace jubatus {

typedef unordered_map<string, uint64_t>::const_iterator cit;

key_manager::key_manager() {
}

key_manager& key_manager::operator =(const key_manager& km) {
  key2id_ = km.key2id_;
  id2key_ = km.id2key_;
  return *this;
}

uint64_t key_manager::get_id(const string& key) {
  cit it = key2id_.find(key);
  if (it != key2id_.end()) {
    return it->second;
  }
  uint64_t new_id = static_cast<uint64_t>(key2id_.size());
  key2id_[key] = new_id;
  id2key_.push_back(key);
  return new_id;
}

uint64_t key_manager::get_id_const(const string& key) const {
  cit it = key2id_.find(key);
  if (it != key2id_.end()) {
    return it->second;
  } else {
    return NOTFOUND;
  }
}

const string& key_manager::get_key(const uint64_t id) const {
  if (id < id2key_.size()) {
    return id2key_[id];
  } else {
    return vacant_;
  }
}

void key_manager::swap(key_manager& km) {
  std::swap(key2id_, km.key2id_);
  id2key_.swap(km.id2key_);
  // no swap for vacant
}

void key_manager::clear() {
  pfi::data::unordered_map<std::string, uint64_t>().swap(key2id_);
  std::vector<std::string>().swap(id2key_);
}

void key_manager::init_by_id2key(const std::vector<std::string>& id2key) {
  key2id_.clear();
  id2key_.clear();
  for (size_t i = 0; i < id2key.size(); ++i) {
    key2id_[id2key[i]] = i;
  }
  id2key_ = id2key;
}

vector<string> key_manager::get_all_id2key() const {
  return id2key_;
}

}  // namespace jubatus
