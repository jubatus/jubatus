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

#include "burst_local_storage.hpp"

#include <string>

using jubatus::util::lang::shared_ptr;
using jubatus::util::lang::weak_ptr;

namespace jubatus {
namespace core {
namespace burst {
namespace storage {

weak_ptr<burst_local_storage> burst_local_storage::burst_local_storage_;

shared_ptr<burst_local_storage> burst_local_storage::get_instance() {
  shared_ptr<burst_local_storage> p = burst_local_storage_.lock();
  if (!p) {
    p.reset(new burst_local_storage());
    burst_local_storage_ = p;
  }
  return p;
}

bool burst_local_storage::is_my_keyword(const std::string& keyword_txt) const {
  return true;
}

}  // namespace storage
}  // namespace burst
}  // namespace core
}  // namespace jubatus
