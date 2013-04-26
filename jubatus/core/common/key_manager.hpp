// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_COMMON_KEY_MANAGER_HPP_
#define JUBATUS_CORE_COMMON_KEY_MANAGER_HPP_

#include <stdint.h>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <pficommon/data/unordered_map.h>
#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>

namespace jubatus {

class key_manager {
 public:
  enum {
    NOTFOUND = 0xFFFFFFFFFFFFFFFFLLU
  };

  key_manager();
  key_manager& operator =(const key_manager&);

  size_t size() const {
    return key2id_.size();
  }

  uint64_t get_id(const std::string& key);
  uint64_t get_id_const(const std::string& key) const;
  const std::string& get_key(const uint64_t id) const;
  void swap(key_manager& km);
  void clear();

  void init_by_id2key(const std::vector<std::string>& id2key);
  std::vector<std::string> get_all_id2key() const;

 protected:
  friend class pfi::data::serialization::access;
  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(key2id_) & MEMBER(id2key_);
  }

 private:
  pfi::data::unordered_map<std::string, uint64_t> key2id_;
  std::vector<std::string> id2key_;
  const std::string vacant_;
};

}  // namespace jubatus

#endif  // JUBATUS_CORE_COMMON_KEY_MANAGER_HPP_
