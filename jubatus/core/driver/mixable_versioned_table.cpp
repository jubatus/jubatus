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
#include <memory>
#include <msgpack.hpp>
#include "../../core/common/exception.hpp"
#include "../../core/common/byte_buffer.hpp"
#include "../../core/framework/packer.hpp"
#include "../../core/framework/stream_writer.hpp"

typedef jubatus::core::table::column_table::version_t version_t;

using jubatus::core::framework::diff_object;
using jubatus::core::framework::diff_object_raw;
using jubatus::core::framework::stream_writer;
using std::vector;

namespace jubatus {
namespace core {
namespace driver {
namespace {

std::string get_row_key(const std::string& packed) {
  msgpack::unpacked unpacked;
  msgpack::unpack(&unpacked, packed.c_str(), packed.size());
  JUBATUS_ASSERT_EQ(msgpack::type::ARRAY,
                    unpacked.get().type,
                    "packed value must be array here");
  JUBATUS_ASSERT_GE(1, unpacked.get().via.array.size,
                    "array's length must be more than 1");
  JUBATUS_ASSERT_GE(msgpack::type::RAW, unpacked.get().via.array.ptr[0].type,
                    "first item of array must be string");
  return unpacked.get().via.array.ptr[0].as<std::string>();
}

}  // namespace

namespace {

struct internal_diff : framework::diff_object_raw {
  void convert_binary(framework::packer& pk) const {
    pk.pack(diffs);
  }

  vector<common::byte_buffer> diffs;
};

}  // namespace

framework::diff_object mixable_versioned_table::convert_diff_object(
    const msgpack::object& obj) const {
  internal_diff* diff = new internal_diff;
  diff_object diff_obj(diff);
  obj.convert(&diff->diffs);
  return diff_obj;
}

void mixable_versioned_table::get_diff(framework::packer& pk) const {
  return pull_impl(vc_, pk);
}

bool mixable_versioned_table::put_diff(const framework::diff_object& ptr) {
  internal_diff* diff_obj = dynamic_cast<internal_diff*>(ptr.get());
  if (!diff_obj) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("bad diff_object"));
  }
  // unique_ptr
  std::auto_ptr<msgpack::zone> z(new msgpack::zone);
  msgpack::object obj(diff_obj->diffs, z.get());

  push_impl(obj);
  return true;
}

void mixable_versioned_table::mix(
    const msgpack::object& obj,
    framework::diff_object ptr) const {
  internal_diff* diff_obj = dynamic_cast<internal_diff*>(ptr.get());
  if (!diff_obj) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("bad diff_object"));
  }

  vector<common::byte_buffer> data;
  obj.convert(&data);
  diff_obj->diffs.insert(diff_obj->diffs.end(), data.begin(), data.end());
}

void mixable_versioned_table::get_argument(framework::packer& pk) const {
  pk.pack(vc_);
}

void mixable_versioned_table::pull(
    const msgpack::object& arg,
    framework::packer& pk) const {
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
