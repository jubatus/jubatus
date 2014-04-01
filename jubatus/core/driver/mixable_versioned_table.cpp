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
#include <msgpack.hpp>
#include "../../core/common/exception.hpp"

typedef jubatus::core::table::column_table::version_t version_t;

namespace jubatus {
namespace core {
namespace driver {
namespace {

std::string get_row_key(const std::string& packed) {
  msgpack::unpacked unpacked;
  msgpack::unpack(&unpacked, packed.c_str(), packed.size());
  return unpacked.get().via.array.ptr[0].as<std::string>();
}

}  // namespace

std::vector<std::string> mixable_versioned_table::get_diff_impl() const {
  return pull_impl(vc_);
}

bool mixable_versioned_table::put_diff_impl(
    const std::vector<std::string>& diff) {
  // TODO(beam2d): Skip rows whose owner is this server.
  push_impl(diff);
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

version_clock mixable_versioned_table::get_pull_argument_impl() const {
  return vc_;
}

std::vector<std::string> mixable_versioned_table::pull_impl(
    const version_clock& vc) const {
  std::vector<std::string> diff;

  model_ptr table = get_model();
  const uint64_t table_size = table->size();
  for (uint64_t i = 0; i < table_size; ++i) {
    const version_t version = table->get_version(i);
    version_clock::const_iterator it = vc.find(version.first);
    if (it == vc.end() || it->second < version.second) {
      diff.push_back(table->get_row(i));
    }
  }
  return diff;
}

void mixable_versioned_table::push_impl(
    const std::vector<std::string>& diff) {
  model_ptr table = get_model();
  for (uint64_t i = 0; i < diff.size(); ++i) {
    unlearner_->touch(get_row_key(diff[i]));
    const version_t version = table->set_row(diff[i]);
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
