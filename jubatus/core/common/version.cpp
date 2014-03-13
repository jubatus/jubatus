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

#include "version.hpp"
#include <ostream>

namespace jubatus {
namespace core {
namespace storage {

std::ostream& operator<<(std::ostream& os, const version& v) {
  os << v.version_number_;
  // cardinal number output
  if ((v.version_number_ % 10) == 1
      && ((v.version_number_ % 100) != 11)) {
    os << "st";
  } else if ((v.version_number_ % 10) == 2
             && ((v.version_number_ % 100) != 12)) {
    os << "nd";
  } else if ((v.version_number_ % 10) == 3
             && ((v.version_number_ % 100) != 13)) {
    os << "rd";
  } else {
    os << "th";
  }
  return os;
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
