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

using std::set;
using std::string;
using std::vector;
using jubatus::core::framework::diff_object;
using jubatus::core::framework::diff_object_raw;
using jubatus::core::framework::mixable;
using jubatus::core::framework::packer;
using jubatus::core::framework::linear_mixable;

namespace jubatus {
namespace core {
namespace driver {

namespace {

struct internal_diff_object : diff_object_raw {
  internal_diff_object(const vector<diff_object>& diffs)
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

int is_linear_mixable(int i, mixable* m) {
  return i + (dynamic_cast<linear_mixable*>(m) ? 1 : 0);
}

}

set<string> driver_base::mixable_holder::mixables() const {
  set<string> s;

  if (mixables_.size() == std::accumulate(mixables_.begin(), mixables_.end(), 0, is_linear_mixable)) {
    s.insert("linear_mixable");
  }

  return s;
}

void driver_base::mixable_holder::register_mixable(mixable* mixable) {
  if (std::find(mixables_.begin(), mixables_.end(), mixable) == mixables_.end()) {
    mixables_.push_back(mixable);
  }
}

diff_object driver_base::mixable_holder::convert_diff_object(const msgpack::object& o) const {
  if (o.type != msgpack::type::ARRAY || o.via.array.size != mixables_.size()) {
    throw msgpack::type_error();
  }

  vector<diff_object> diffs;
  for (size_t i = 0; i < mixables_.size(); i++) {
    const linear_mixable* mixable = dynamic_cast<const linear_mixable*>(mixables_[i]);
    if (!mixable) {
      throw JUBATUS_EXCEPTION(core::common::exception::runtime_error("does not support linear_mixable"));
    }
    diffs.push_back(mixable->convert_diff_object(o.via.array.ptr[i]));
  }

  return diff_object(new internal_diff_object(diffs));
}

void driver_base::mixable_holder::mix(const msgpack::object& o, diff_object ptr) const {
  if (o.type != msgpack::type::ARRAY || o.via.array.size != mixables_.size()) {
    throw msgpack::type_error();
  }

  internal_diff_object* diff_obj = dynamic_cast<internal_diff_object*>(ptr.get());
  if (!diff_obj) {
    throw JUBATUS_EXCEPTION(core::common::exception::runtime_error("bad diff_object"));
  }

  if (mixables_.size() != diff_obj->diffs_.size()) {
    throw JUBATUS_EXCEPTION(core::common::exception::runtime_error("diff size is wrong"));
  }

  for (size_t i = 0; i < mixables_.size(); i++) {
    const linear_mixable* mixable = dynamic_cast<const linear_mixable*>(mixables_[i]);
    if (!mixable) {
      throw JUBATUS_EXCEPTION(core::common::exception::runtime_error("does not support linear_mixable"));
    }
    mixable->mix(o.via.array.ptr[i], diff_obj->diffs_[i]);
  }
}

void driver_base::mixable_holder::get_diff(packer& pk) const {
  for (size_t i = 0; i < mixables_.size(); i++) {
    const linear_mixable* mixable = dynamic_cast<const linear_mixable*>(mixables_[i]);
    if (!mixable) {
      throw JUBATUS_EXCEPTION(core::common::exception::runtime_error("does not support linear_mixable"));
    }
    mixable->get_diff(pk);
  }
}

bool driver_base::mixable_holder::put_diff(const diff_object& obj) {
  internal_diff_object* diff_obj = dynamic_cast<internal_diff_object*>(obj.get());
  if (!diff_obj) {
    throw JUBATUS_EXCEPTION(core::common::exception::runtime_error("bad diff_object"));
  }

  if (mixables_.size() != diff_obj->diffs_.size()) {
    throw JUBATUS_EXCEPTION(core::common::exception::runtime_error("diff size is wrong"));
  }

  bool success = true;
  for (size_t i = 0; i < mixables_.size(); i++) {
    linear_mixable* mixable = dynamic_cast<linear_mixable*>(mixables_[i]);
    if (!mixable) {
      throw JUBATUS_EXCEPTION(core::common::exception::runtime_error("does not support linear_mixable"));
    }
    success = mixable->put_diff(diff_obj->diffs_[i]) && success;
  }

  return success;
}

void driver_base::register_mixable(framework::mixable* mixable) {
  holder_.register_mixable(mixable);
}


}  // namespace driver
}  // namespace core
}  // namespace jubatus
