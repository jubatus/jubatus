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

#include "storage_factory.hpp"

#include <string>

#include "../common/exception.hpp"
#include "storage_base.hpp"
#include "local_storage.hpp"
#include "local_storage_mixture.hpp"

namespace jubatus {
namespace core {
namespace storage {

storage_base* storage_factory::create_storage(const std::string& name) {
  if (name == "local") {
    return static_cast<storage_base*>(new local_storage);
  } else if (name == "local_mixture") {
    return static_cast<storage_base*>(new local_storage_mixture);
  }

  // maybe bug or configuration mistake
  throw JUBATUS_EXCEPTION(
      jubatus::exception::runtime_error(
          std::string("failed to create storage: ") + name));
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
