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

#include "driver.hpp"
#include <algorithm>
#include <numeric>
#include <string>
#include <set>
#include <vector>

using std::find;
using std::set;
using std::string;
using std::vector;
using jubatus::core::framework::diff_object;
using jubatus::core::framework::diff_object_raw;
using jubatus::core::framework::mixable;
using jubatus::core::framework::packer;
using jubatus::core::framework::linear_mixable;
using jubatus::core::framework::push_mixable;

namespace jubatus {
namespace core {
namespace driver {

namespace {

struct internal_diff_object : diff_object_raw {
  explicit internal_diff_object(const vector<diff_object>& diffs)
    : diffs_(diffs) {
  }

  void convert_binary(packer& pk) const {
    pk.pack_array(diffs_.size());
    for (size_t i = 0; i < diffs_.size(); i++) {
      diffs_[i]->convert_binary(pk);
    }
  }

  vector<diff_object> diffs_;
};

template <class T>
size_t count_mixable(const vector<mixable*>& mixables) {
  size_t count = 0;
  for (size_t i = 0; i < mixables.size(); i++) {
    if (dynamic_cast<T*>(mixables[i])) {
      count++;
    }
  }

  return count;
}

}  // namespace

set<string> driver_base::mixable_holder::mixables() const {
  set<string> s;

  if (count_mixable<linear_mixable>(mixables_) > 0) {
    s.insert("linear_mixable");
  }

  if (count_mixable<push_mixable>(mixables_) > 0) {
    s.insert("push_mixable");
  }

  return s;
}

void driver_base::mixable_holder::register_mixable(mixable* mixable) {
  if (find(mixables_.begin(), mixables_.end(), mixable) == mixables_.end()) {
    mixables_.push_back(mixable);
  }
}

diff_object driver_base::mixable_holder::convert_diff_object(
    const msgpack::object& o) const {
  if (o.type != msgpack::type::ARRAY ||
      o.via.array.size != count_mixable<linear_mixable>(mixables_)) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("conversion failed"));
  }

  vector<diff_object> diffs;
  for (size_t i = 0; i < mixables_.size(); i++) {
    const linear_mixable* mixable =
      dynamic_cast<const linear_mixable*>(mixables_[i]);
    if (!mixable) {
      continue;
    }
    diffs.push_back(mixable->convert_diff_object(o.via.array.ptr[i]));
  }

  return diff_object(new internal_diff_object(diffs));
}

void driver_base::mixable_holder::mix(
    const msgpack::object& o,
    diff_object ptr) const {
  if (o.type != msgpack::type::ARRAY ||
      o.via.array.size != count_mixable<linear_mixable>(mixables_)) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("mix failed"));
  }

  internal_diff_object* diff_obj =
    dynamic_cast<internal_diff_object*>(ptr.get());
  if (!diff_obj) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("bad diff_object"));
  }

  if (mixables_.size() != diff_obj->diffs_.size()) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("diff size is wrong"));
  }

  for (size_t i = 0; i < mixables_.size(); i++) {
    const linear_mixable* mixable =
      dynamic_cast<const linear_mixable*>(mixables_[i]);
    if (!mixable) {
      continue;
    }
    mixable->mix(o.via.array.ptr[i], diff_obj->diffs_[i]);
  }
}

void driver_base::mixable_holder::get_diff(packer& pk) const {
  pk.pack_array(count_mixable<linear_mixable>(mixables_));
  for (size_t i = 0; i < mixables_.size(); i++) {
    const linear_mixable* mixable =
      dynamic_cast<const linear_mixable*>(mixables_[i]);
    if (!mixable) {
      continue;
    }
    mixable->get_diff(pk);
  }
}

bool driver_base::mixable_holder::put_diff(const diff_object& obj) {
  internal_diff_object* diff_obj =
    dynamic_cast<internal_diff_object*>(obj.get());
  if (!diff_obj) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("bad diff_object"));
  }

  if (count_mixable<linear_mixable>(mixables_) != diff_obj->diffs_.size()) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("diff size is wrong"));
  }

  bool success = true;
  for (size_t i = 0; i < mixables_.size(); i++) {
    linear_mixable* mixable = dynamic_cast<linear_mixable*>(mixables_[i]);
    if (!mixable) {
      continue;
    }
    success = mixable->put_diff(diff_obj->diffs_[i]) && success;
  }

  return success;
}

void driver_base::mixable_holder::get_argument(packer& pk) const {
  pk.pack_array(count_mixable<push_mixable>(mixables_));
  for (size_t i = 0; i < mixables_.size(); i++) {
    const push_mixable* mixable =
      dynamic_cast<const push_mixable*>(mixables_[i]);
    if (!mixable) {
      continue;
    }
    mixable->get_argument(pk);
  }
}

void driver_base::mixable_holder::pull(
    const msgpack::object& arg,
    packer& pk) const {
  if (arg.type != msgpack::type::ARRAY ||
      arg.via.array.size != count_mixable<push_mixable>(mixables_)) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("pull array failed"));
  }

  pk.pack_array(count_mixable<push_mixable>(mixables_));
  for (size_t i = 0, obj_index = 0; i < mixables_.size(); i++) {
    const push_mixable* mixable =
      dynamic_cast<const push_mixable*>(mixables_[i]);
    if (!mixable) {
      continue;
    }
    mixable->pull(arg.via.array.ptr[obj_index], pk);
    obj_index++;
  }
}

void driver_base::mixable_holder::push(const msgpack::object& o) {
  if (o.type != msgpack::type::ARRAY ||
      o.via.array.size != count_mixable<push_mixable>(mixables_)) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("push failed"));
  }

  for (size_t i = 0, obj_index = 0; i < mixables_.size(); i++) {
    push_mixable* mixable = dynamic_cast<push_mixable*>(mixables_[i]);
    if (!mixable) {
      continue;
    }
    mixable->push(o.via.array.ptr[obj_index]);
    obj_index++;
  }
}

std::vector<storage::version>
driver_base::mixable_holder::get_versions() const {
  std::vector<storage::version> ret;
  for (size_t i = 0; i < mixables_.size(); ++i) {
    ret.push_back(mixables_[i]->get_version());
  }
  return ret;
}

std::vector<storage::version> driver_base::get_versions() const {
  return holder_.get_versions();
}

void driver_base::register_mixable(framework::mixable* mixable) {
  holder_.register_mixable(mixable);
}


}  // namespace driver
}  // namespace core
}  // namespace jubatus
