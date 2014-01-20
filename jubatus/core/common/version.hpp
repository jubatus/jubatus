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

#include <stdint.h>
#include <msgpack.hpp>
#include "jubatus/util/data/serialization.h"

namespace jubatus {
namespace core {
namespace storage {

class version {
 public:
  version();
  void increment();
  uint64_t get_version() const;
  MSGPACK_DEFINE(version_number_);

 private:
  uint64_t version_number_;

  friend class jubatus::util::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(version_number_);
  }
};

}  // namespace storage
}  // namespace core
}  // namespace jubatus
