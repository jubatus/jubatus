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

#include "lru_unlearner.hpp"

#include <string>
#include "jubatus/util/data/serialization.h"
#include "../common/exception.hpp"

namespace jubatus {
namespace core {
namespace unlearner {

lru_unlearner::lru_unlearner(const config& conf)
    : max_size_(conf.max_size) {
  if (conf.max_size <= 0) {
    throw JUBATUS_EXCEPTION(
        common::config_exception() << common::exception::error_message(
            "max_size must be a positive integer"));
  }
  entry_map_.reserve(max_size_);
}

void lru_unlearner::touch(const std::string& id) {
  entry_map::iterator it = entry_map_.find(id);
  if (it != entry_map_.end()) {
    lru_.erase(it->second);
    lru_.push_front(id);
    it->second = lru_.begin();
    return;
  }

  if (entry_map_.size() == max_size_) {
    // Unlearn the least recently used entry.
    unlearn(lru_.back());
    entry_map_.erase(lru_.back());
    lru_.pop_back();
  }

  lru_.push_front(id);
  entry_map_[id] = lru_.begin();
}

bool lru_unlearner::exists_in_memory(const std::string& id) const {
  return entry_map_.count(id) > 0;
}

void lru_unlearner::save(std::ostream& os) const {
  jubatus::util::data::serialization::binary_oarchive bo(os);
  bo << const_cast<lru_unlearner&>(*this);
}

void lru_unlearner::load(std::istream& is) {
  jubatus::util::data::serialization::binary_iarchive bi(is);
  bi >> *this;
}

// private

void lru_unlearner::rebuild_entry_map() {
  entry_map_.clear();
  for (lru::iterator it = lru_.begin(); it != lru_.end(); ++it) {
    entry_map_[*it] = it;
  }
}

}  // namespace unlearner
}  // namespace core
}  // namespace jubatus
