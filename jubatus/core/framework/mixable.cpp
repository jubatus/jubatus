// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "mixable.hpp"
#include <string>
#include <set>

namespace jubatus {
namespace core {
namespace framework {

mixable::mixable() {
}

mixable::mixable(const std::string& name) {
  mixables_.insert(name);
}

mixable::~mixable() {
}

std::set<std::string> mixable::mixables() const {
  return mixables_;
}

storage::version mixable::get_version() const {
  return storage::version();
}

}  // namespace framework
}  // namespace core
}  // namespace jubatus
