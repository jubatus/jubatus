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

#include "simple_storage.hpp"

#include <string>
#include <vector>

namespace jubatus {
namespace core {
namespace clustering {

simple_storage::simple_storage(
    const std::string& name,
    const clustering_config& config)
    : storage(name, config) {
}

void simple_storage::add(const weighted_point& point) {
  static size_t cnt = 0;
  ++cnt;
  if (cnt % config_.backet_size == 0) {
    increment_revision();
  }
  mine_.push_back(point);
}

wplist simple_storage::get_mine() const {
  return mine_;
}

void simple_storage::pack(msgpack::packer<msgpack::sbuffer>& packer) const {
  packer.pack_array(2);
  packer.pack(static_cast<const storage&>(*this));
  packer.pack(mine_);
}
void simple_storage::unpack(msgpack::object o) {
  std::vector<msgpack::object> mems;
  o.convert(&mems);
  if (mems.size() != 2) {
    throw msgpack::type_error();
  }
  mems[0].convert(static_cast<storage*>(this));
  mems[1].convert(&mine_);
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
