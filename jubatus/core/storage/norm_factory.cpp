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

#include "norm_factory.hpp"

#include <string>
#include "norm.hpp"
#include "../common/exception.hpp"

using std::string;

namespace jubatus {
namespace core {
namespace storage {

norm_base* create_norm(const string& name) {
  if (name == "l2") {
    return new norm_l2;
  } else if (name == "l1") {
    return new norm_l1;
  } else {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error(
            string("create_norm unknown name error:") + name));
  }
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
