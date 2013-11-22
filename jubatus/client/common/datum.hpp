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

#ifndef JUBATUS_CLIENT_COMMON_DATUM_HPP_
#define JUBATUS_CLIENT_COMMON_DATUM_HPP_

#include <string>
#include <vector>
#include <utility>
#include <msgpack.hpp>

namespace jubatus {
namespace client {
namespace common {

struct datum {
  void add_number(const std::string& key, float value) {
    num_values.push_back(std::make_pair(key, value));
  }

  void add_string(const std::string& key, const std::string& value) {
    string_values.push_back(std::make_pair(key, value));
  }

  void add_binary(const std::string& key, const std::string& value) {
    binary_values.push_back(std::make_pair(key, value));
  }

  MSGPACK_DEFINE(string_values, num_values, binary_values);

  std::vector<std::pair<std::string, std::string> > string_values;
  std::vector<std::pair<std::string, double> > num_values;
  std::vector<std::pair<std::string, std::string> > binary_values;
};

}  // namespace common
}  // namespace client
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_COMMON_DATUM_HPP_
