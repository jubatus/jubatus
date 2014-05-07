// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012,2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_TABLE_COLUMN_OWNER_HPP_
#define JUBATUS_CORE_TABLE_COLUMN_OWNER_HPP_

#include <ostream>
#include <string>
#include <msgpack.hpp>

namespace jubatus {
namespace core {
namespace table {

struct owner {
  std::string name;

  owner() : name("(uninitialized)") {
  }
  explicit owner(const std::string& n) : name(n) {
  }

  bool operator==(const owner& rhs) const {
    return name == rhs.name;
  }
  bool operator<(const owner& o) const {
    return name < o.name;
  }
  friend std::ostream& operator<<(std::ostream& os, const owner& o) {
    os << "owner[" << o.name << "]";
    return os;
  }

  MSGPACK_DEFINE(name);
};

}  // namespace table
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_TABLE_COLUMN_OWNER_HPP_
