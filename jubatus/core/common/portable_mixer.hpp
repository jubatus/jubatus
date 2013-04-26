// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_COMMON_PORTABLE_MIXER_HPP_
#define JUBATUS_CORE_COMMON_PORTABLE_MIXER_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <pficommon/math/random.h>
#include "../common/hash.hpp"

namespace jubatus {

template <typename Storage>
class portable_mixer {
 public:
  portable_mixer() {
  }

  explicit portable_mixer(uint32_t seed)
      : rand_(seed) {
  }

  void clear() {
    storages_.clear();
  }

  void add(Storage* storage) {
    storages_.push_back(storage);
  }

  Storage* get_random() {
    return storages_[rand_(storages_.size())];
  }

  Storage* get_hash(const std::string& id) const {
    return storages_[hash_util::calc_string_hash(id) % storages_.size()];
  }

  void mix() {
    if (storages_.empty()) {
      return;
    }

    std::string mixed;
    storages_[0]->get_diff(mixed);

    for (size_t i = 1; i < storages_.size(); ++i) {
      std::string diff;
      storages_[i]->get_diff(diff);
      storages_[0]->mix(diff, mixed);
    }

    for (size_t i = 0; i < storages_.size(); ++i) {
      storages_[i]->set_mixed_and_clear_diff(mixed);
    }
  }

 private:
  std::vector<Storage*> storages_;
  pfi::math::random::mtrand rand_;
};

}  // namespace jubatus

#endif  // JUBATUS_CORE_COMMON_PORTABLE_MIXER_HPP_
