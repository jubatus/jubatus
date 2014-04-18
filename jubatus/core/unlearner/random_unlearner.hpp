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

#ifndef JUBATUS_CORE_UNLEARNER_RANDOM_UNLEARNER_HPP_
#define JUBATUS_CORE_UNLEARNER_RANDOM_UNLEARNER_HPP_

#include <string>
#include <vector>
#include "jubatus/util/data/optional.h"
#include "jubatus/util/data/serialization.h"
#include "jubatus/util/data/unordered_set.h"
#include "jubatus/util/math/random.h"
#include "unlearner_base.hpp"

namespace jubatus {
namespace core {
namespace unlearner {

// Unlearner that chooses an item to be removed by uniformly random sampling.
class random_unlearner : public unlearner_base {
 public:
  struct config {
    int32_t max_size;
    jubatus::util::data::optional<int32_t> seed;

    template<typename Ar>
    void serialize(Ar& ar) {
      ar & JUBA_MEMBER(max_size) & JUBA_MEMBER(seed);
    }
  };

  std::string type() const {
    return "random_unlearner";
  }

  void clear() {
    id_set_.clear();
    ids_.clear();
  }

  explicit random_unlearner(const config& conf);

  void touch(const std::string& id);
  bool exists_in_memory(const std::string& id) const;

  void save(std::ostream& os) const;
  void load(std::istream& is);

 private:
  friend class jubatus::util::data::serialization::access;
  template<typename Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(ids_) & JUBA_MEMBER(max_size_);

    if (ar.is_read) {
      id_set_.clear();
      for (size_t i = 0; i < ids_.size(); ++i) {
        id_set_.insert(ids_[i]);
      }
    }
  }

  jubatus::util::data::unordered_set<std::string> id_set_;
  std::vector<std::string> ids_;
  size_t max_size_;

  jubatus::util::math::random::mtrand mtr_;
};

}  // namespace unlearner
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_UNLEARNER_RANDOM_UNLEARNER_HPP_

