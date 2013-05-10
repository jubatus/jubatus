// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "config.hpp"

#include <iostream>
#include <string>

#include "exception.hpp"

namespace jubatus {
namespace core {
namespace jsonconfig {

config::iterator::iterator(const config::iterator& it)
    : parent_(it.parent_),
      it_(it.it_) {
}

config::iterator::iterator(
    const config& parent,
    const pfi::text::json::json::const_iterator& it)
    : parent_(parent),
      it_(it) {
}

const std::string& config::iterator::iterator::key() const {
  return it_->first;
}

config config::iterator::value() const {
  return config(it_->second, parent_.path() + "." + key());
}

config config::operator[](size_t index) const {
  try {
    if (index < json_.size()) {
      std::ostringstream os;
      os << path_ << "[" << index << "]";
      return config(json_[index], os.str());
    } else {
      throw JUBATUS_EXCEPTION(out_of_range(path_, json_.size(), index));
    }
  } catch (const std::bad_cast& e) {
    throw JUBATUS_EXCEPTION(
        type_error(path_, pfi::text::json::json::Array, type()));
  }
}

config config::operator[](const std::string& key) const {
  try {
    std::ostringstream os;
    os << path_ << "." << key;
    return config(json_[key], os.str());
  } catch (const std::out_of_range& e) {
    throw JUBATUS_EXCEPTION(not_found(path_, key));
  } catch (const std::bad_cast& e) {
    throw JUBATUS_EXCEPTION(
        type_error(path_, pfi::text::json::json::Object, type()));
  }
}

bool config::contain(const std::string& key) const {
  if (type() != pfi::text::json::json::Object) {
    throw JUBATUS_EXCEPTION(
        type_error(path_, pfi::text::json::json::Object, type()));
  }
  return json_.count(key) > 0;
}

size_t config::size() const {
  try {
    return json_.size();
  } catch (const std::bad_cast& e) {
    throw JUBATUS_EXCEPTION(
        type_error(path_, pfi::text::json::json::Array, type()));
  }
}

config::iterator config::begin() const {
  return iterator(*this, json_.begin());
}

config::iterator config::end() const {
  return iterator(*this, json_.end());
}

}  // namespace jsonconfig
}  // namespace core
}  // namespace jubatus
