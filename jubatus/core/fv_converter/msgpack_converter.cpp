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

#include <sstream>
#include <string>
#include <msgpack.hpp>
#include "datum.hpp"
#include "msgpack_converter.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

namespace {

const char* NULL_VALUE = "NULL";

void iter_convert(
    const msgpack::object& object,
    const std::string& path,
    datum& datum) {
  switch (object.type) {
    case msgpack::type::NIL: {
      datum.string_values_.push_back(std::make_pair(path, NULL_VALUE));
      break;
    }

    case msgpack::type::BOOLEAN: {
      double v = object.via.boolean ? 1. : 0.;
      datum.num_values_.push_back(std::make_pair(path, v));
      break;
    }

    case msgpack::type::POSITIVE_INTEGER: {
      double v = object.via.u64;
      datum.num_values_.push_back(std::make_pair(path, v));
      break;
    }

    case msgpack::type::NEGATIVE_INTEGER: {
      double v = object.via.i64;
      datum.num_values_.push_back(std::make_pair(path, v));
      break;
    }

    case msgpack::type::DOUBLE: {
      double v = object.via.dec;
      datum.num_values_.push_back(std::make_pair(path, v));
      break;
    }

    case msgpack::type::RAW: {
      const msgpack::object_raw& raw = object.via.raw;
      datum.string_values_.push_back(
          std::make_pair(path, std::string(raw.ptr, raw.size)));
      break;
    }

    case msgpack::type::ARRAY: {
      const msgpack::object_array& array = object.via.array;
      for (uint32_t i = 0; i < array.size; ++i) {
        std::ostringstream oss;
        oss << path << "[" << i << "]";
        iter_convert(array.ptr[i], oss.str(), datum);
      }
      break;
    }

    case msgpack::type::MAP: {
      const msgpack::object_map& map = object.via.map;
      for (uint32_t i = 0; i < map.size; ++i) {
        const msgpack::object_kv& kv = map.ptr[i];
        std::ostringstream oss;
        oss << path << "/" << kv.key;
        iter_convert(kv.val, oss.str(), datum);
      }
    }
  }
}

}  // namespace

void msgpack_converter::convert(const msgpack::object& object, datum& datum) {
  iter_convert(object, "", datum);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
