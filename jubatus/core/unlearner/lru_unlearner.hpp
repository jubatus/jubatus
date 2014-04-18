// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_UNLEARNER_LRU_UNLEARNER_HPP_
#define JUBATUS_CORE_UNLEARNER_LRU_UNLEARNER_HPP_

#include <stdint.h>
#include <list>
#include <string>
#include "jubatus/util/data/serialization.h"
#include "jubatus/util/data/unordered_map.h"
#include "unlearner_base.hpp"

namespace jubatus {
namespace core {
namespace unlearner {

// Unlearner based on Least Recently Used algorithm.
class lru_unlearner : public unlearner_base {
 public:
  struct config {
    int32_t max_size;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & JUBA_MEMBER(max_size);
    }
  };

  std::string type() const {
    return "lru_unlearner";
  }

  void clear() {
    lru_.clear();
    entry_map_.clear();
  }

  explicit lru_unlearner(const config& conf);

  void touch(const std::string& id);
  bool exists_in_memory(const std::string& id) const;

  void save(std::ostream& os) const;
  void load(std::istream& is);

 private:
  typedef std::list<std::string> lru;
  typedef jubatus::util::data::unordered_map<std::string, lru::iterator> entry_map;

  friend class jubatus::util::data::serialization::access;
  template<typename Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(lru_) & JUBA_MEMBER(max_size_);

    if (ar.is_read) {
      rebuild_entry_map();
    }
  }

  void rebuild_entry_map();

  lru lru_;
  entry_map entry_map_;
  size_t max_size_;
};

}  // namespace unlearner
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_UNLEARNER_LRU_UNLEARNER_HPP_
