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

void mixable_holder::mix(const std::vector<msgpack::object>& objs, packer& pk) {
  std::vector<linear_mixable*> mixables;
  for (size_t i = 0; i < mixables_.size(); i++) {
    linear_mixable* mixable = dynamic_cast<linear_mixable*>(mixables_[i].get());
    mixables.push_back(mixable);
  }

  for (size_t i = 0; i < objs.size(); i++) {
    if (objs[i].type != msgpack::type::ARRAY || objs[i].via.array.size != mixables.size()) {
      //throw msgpack::type_error();
      throw JUBATUS_EXCEPTION(common::exception::runtime_error("size: " + 
        jubatus::util::lang::lexical_cast<std::string>(objs[i].via.array.size) + " mixables: " +
        jubatus::util::lang::lexical_cast<std::string>(mixables.size())
        ));
    }
  }

  // Convert front msgpack::object to diff_object
  std::vector<diff_object> diffs;
  for (size_t i = 0; i < mixables.size(); i++) {
    diffs.push_back(mixables[i]->convert_diff_object(objs[0].via.array.ptr[i]));
  }

  // do mix
  for (size_t i = 1; i < objs.size(); i++) {
    const msgpack::object* o = objs[i].via.array.ptr;
    for (size_t j = 0; j < mixables.size(); j++) {
      mixables[j]->mix(o[j], diffs[j]);
    }
  }

  // output mixed buffer
  pk.pack_array(mixables.size());
  for (size_t i = 0; i < mixables.size(); i++) {
    diffs[i]->convert_binary(pk);
  }
}


}  // namespace framework
}  // namespace core
}  // namespace jubatus
