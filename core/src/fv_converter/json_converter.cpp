// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "json_converter.hpp"

#include <sstream>
#include <string>
#include <pficommon/text/json.h>
#include "datum.hpp"

using pfi::text::json::json_array;
using pfi::text::json::json_bool;
using pfi::text::json::json_float;
using pfi::text::json::json_integer;
using pfi::text::json::json_null;
using pfi::text::json::json_object;
using pfi::text::json::json_string;

namespace jubatus {
namespace core {
namespace fv_converter {

const char* json_converter::NULL_STRING = "null";

namespace {

void iter_convert(
    const pfi::text::json::json& json,
    std::string& root_path,
    datum& ret_datum);

void convert_integer(
    const json_integer& value,
    const std::string& path,
    datum& ret_datum) {
  ret_datum.num_values_.push_back(std::make_pair(path, value.get()));
}

void convert_float(
    const json_float& value,
    const std::string& path,
    datum& ret_datum) {
  ret_datum.num_values_.push_back(std::make_pair(path, value.get()));
}

void convert_string(
    const json_string& value,
    const std::string& path,
    datum& ret_datum) {
  ret_datum.string_values_.push_back(std::make_pair(path, value.get()));
}

void convert_bool(
    const json_bool& value,
    const std::string& path,
    datum& ret_datum) {
  double v = value.get() ? 1 : 0;
  ret_datum.num_values_.push_back(std::make_pair(path, v));
}

void convert_null(
    const json_null& value,
    const std::string& path,
    datum& ret_datum) {
  ret_datum.string_values_.push_back(
      std::make_pair(path, json_converter::NULL_STRING));
}

void convert_array(
    const pfi::text::json::json_array& value,
    std::string& path,
    datum& ret_datum) {
  size_t len = path.size();
  for (size_t i = 0; i < value.size(); ++i) {
    std::ostringstream oss;
    oss << "[" << i << "]";
    path += oss.str();
    iter_convert(value[i], path, ret_datum);
    path.resize(len);
  }
}

void convert_object(
    const json_object& value,
    std::string& path,
    datum& ret_datum) {
  size_t len = path.size();
  for (json_object::const_iterator it = value.begin(); it != value.end();
       ++it) {
    const std::string& key = it->first;
    const pfi::text::json::json& val = it->second;
    path += '/';
    path += key;
    iter_convert(val, path, ret_datum);
    path.resize(len);
  }
}

void iter_convert(
    const pfi::text::json::json& json,
    std::string& root_path,
    datum& ret_datum) {
  pfi::text::json::json_value* value = json.get();
  if (typeid(*value) == typeid(json_integer)) {
    json_integer* int_value = dynamic_cast<json_integer*>(value);
    convert_integer(*int_value, root_path, ret_datum);

  } else if (typeid(*value) == typeid(json_float)) {
    json_float* float_value = dynamic_cast<json_float*>(value);
    convert_float(*float_value, root_path, ret_datum);

  } else if (typeid(*value) == typeid(json_string)) {
    json_string* string_value = dynamic_cast<json_string*>(value);
    convert_string(*string_value, root_path, ret_datum);

  } else if (typeid(*value) == typeid(json_bool)) {
    json_bool* bool_value = dynamic_cast<json_bool*>(value);
    convert_bool(*bool_value, root_path, ret_datum);

  } else if (typeid(*value) == typeid(json_null)) {
    json_null* null_value = dynamic_cast<json_null*>(value);
    convert_null(*null_value, root_path, ret_datum);

  } else if (typeid(*value) == typeid(pfi::text::json::json_array)) {
    json_array* array_value = dynamic_cast<json_array*>(value);
    convert_array(*array_value, root_path, ret_datum);

  } else if (typeid(*value) == typeid(json_object)) {
    json_object* object_value = dynamic_cast<json_object*>(value);
    convert_object(*object_value, root_path, ret_datum);
  }
}

}  // namespace

void json_converter::convert(
    const pfi::text::json::json& json,
    datum& ret_datum) {
  std::string path = "";
  iter_convert(json, path, ret_datum);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
