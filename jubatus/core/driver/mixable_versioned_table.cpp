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

#include "mixable_versioned_table.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include "../../core/common/exception.hpp"

typedef jubatus::core::table::column_table::version_t version_t;

namespace jubatus {
namespace core {
namespace driver {

std::vector<std::string> mixable_versioned_table::get_diff_impl() const {
  //return pull_impl(vc_);
  // FIXME: support linear_mixable
  return std::vector<std::string>();
}

bool mixable_versioned_table::put_diff_impl(
    const std::vector<std::string>& diff) {
  // TODO(beam2d): Skip rows whose owner is this server.
  // FIXME: support linear_mixable
  //push_impl(diff);
  return true;
}

void mixable_versioned_table::mix_impl(
    const std::vector<std::string>& lhs,
    const std::vector<std::string>& rhs,
    std::vector<std::string>& mixed) const {
  mixed.resize(lhs.size() + rhs.size());
  copy(lhs.begin(), lhs.end(), mixed.begin());
  copy(rhs.begin(), rhs.end(), mixed.begin() + lhs.size());
}

void mixable_versioned_table::get_argument(framework::packer& pk) const {
  pk.pack(vc_);
}

void mixable_versioned_table::pull(const msgpack::object& arg, framework::packer& pk) const {
  version_clock vc;
  arg.convert(&vc);
  pull_impl(vc, pk);
}

void mixable_versioned_table::push(const msgpack::object& diff) {
  push_impl(diff);
}

void mixable_versioned_table::pull_impl(
    const version_clock& vc, framework::packer& pk) const {

  model_ptr table = get_model();
  const uint64_t table_size = table->size();
  pk.pack_array(table_size);
  for (uint64_t i = 0; i < table_size; ++i) {
    const version_t version = table->get_version(i);
    version_clock::const_iterator it = vc.find(version.first);
    if (it == vc.end() || it->second < version.second) {
      table->get_row(i, pk);
    }
  }
}

void mixable_versioned_table::push_impl(
    const msgpack::object& o) {
  model_ptr table = get_model();
  const uint64_t table_size = table->size();
  if (o.type != msgpack::type::ARRAY) {
    throw msgpack::type_error();
  }
  for (uint64_t i = 0; i < table_size; ++i) {
    const version_t version = table->set_row(o.via.array.ptr[i]);
    update_version(version);
  }
}

void mixable_versioned_table::update_version(const version_t& version) {
  version_clock::iterator it = vc_.find(version.first);
  if (it == vc_.end()) {
    vc_.insert(version);
  } else if (it->second < version.second) {
    it->second = version.second;
  }
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
