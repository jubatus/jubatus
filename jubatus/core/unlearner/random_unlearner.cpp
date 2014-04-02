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

#include "random_unlearner.hpp"

#include <string>
#include "jubatus/util/data/serialization.h"
#include "../common/exception.hpp"

namespace jubatus {
namespace core {
namespace unlearner {

random_unlearner::random_unlearner(const config& conf)
    : max_size_(conf.max_size) {
  if (conf.max_size <= 0) {
    throw JUBATUS_EXCEPTION(
        common::config_exception() << common::exception::error_message(
            "max_size must be a positive integer"));
  }
  if (conf.seed) {
    mtr_ = jubatus::util::math::random::mtrand(*conf.seed);
  }
  id_set_.reserve(max_size_);
  ids_.reserve(max_size_);
}

void random_unlearner::touch(const std::string& id) {
  if (id_set_.count(id) > 0) {
    return;
  }

  if (id_set_.size() < max_size_) {
    ids_.push_back(id);
  } else {
    const size_t replace_at = mtr_(id_set_.size());
    unlearn(ids_[replace_at]);
    id_set_.erase(ids_[replace_at]);
    ids_[replace_at] = id;
  }
  id_set_.insert(id);
}

bool random_unlearner::exists_in_memory(const std::string& id) const {
  return id_set_.count(id) > 0;
}

void random_unlearner::save(std::ostream& os) const {
  jubatus::util::data::serialization::binary_oarchive bo(os);
  bo << const_cast<random_unlearner&>(*this);
}

void random_unlearner::load(std::istream& is) {
  jubatus::util::data::serialization::binary_iarchive bi(is);
  bi >> *this;
}

}  // namespace unlearner
}  // namespace core
}  // namespace jubatus
