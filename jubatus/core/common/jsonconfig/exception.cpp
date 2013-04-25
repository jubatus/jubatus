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

#include "exception.hpp"

#include <string>
#include <vector>

using std::string;
using pfi::text::json::json;

namespace jubatus {
namespace core {
namespace jsonconfig {

config_error::config_error(const std::string& path, const std::string& message)
    : path_(path),
      message_(message + " (" + path_ + ")") {
}

config_error::~config_error() throw () {
}

namespace {

const char* TypeToName(json::json_type_t t) {
  switch (t) {
    case json::Null:
      return "Null";
    case json::Integer:
      return "Integer";
    case json::Float:
      return "Float";
    case json::Bool:
      return "Bool";
    case json::String:
      return "String";
    case json::Array:
      return "Array";
    case json::Object:
      return "Object";
    default:
      return "Unknown Type";
  }
}

std::string MakeTypeErrorMessage(
    json::json_type_t expect,
    json::json_type_t actual) {
  return string(TypeToName(expect)) + " is expected, but " + TypeToName(actual)
      + " is given.";
}

std::string MakeOutOfRangeMessage(size_t size, size_t index) {
  std::ostringstream os;
  os << "Out of range 0.." << size << ": " << index;
  return os.str();
}

std::string MakeNotFoundMessage(const std::string& key) {
  return "\"" + key + "\" is not found";
}

}  // namespace

type_error::type_error(
    const std::string& path,
    json::json_type_t expect,
    json::json_type_t actual)
    : config_error(path, MakeTypeErrorMessage(expect, actual)),
      expect_(expect),
      actual_(actual) {
}

type_error::~type_error() throw () {
}

out_of_range::out_of_range(const std::string& path, size_t size, size_t index)
    : config_error(path, MakeOutOfRangeMessage(size, index)),
      size_(size),
      index_(index) {
}

out_of_range::~out_of_range() throw () {
}

not_found::not_found(const std::string& path, const std::string& key)
    : config_error(path, MakeNotFoundMessage(key)),
      key_(key) {
}

not_found::~not_found() throw () {
}

cast_check_error::cast_check_error(
    const std::vector<pfi::lang::shared_ptr<config_error> >& errors)
    : errors_(errors.begin(), errors.end()) {
}

cast_check_error::~cast_check_error() throw () {
}

}  // namespace jsonconfig
}  // namespace core
}  // namespace jubatus
