// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include <cstdlib>
#include <map>
#include <string>
#include <stdexcept>

namespace {
std::string host() {
  return std::getenv("JUBATUS_HOST");
}

unsigned int port() {
  char* str = std::getenv("JUBATUS_PORT");
  if (!str) {
    throw std::runtime_error("JUBATUS_PORT is unset");
  }
  return std::strtoul(str, NULL, 0);
}

std::string cluster_name() {
  return std::getenv("JUBATUS_CLUSTER_NAME");
}

unsigned int timeout() {
  char* str = std::getenv("TIMEOUT");
  if (!str) {
    return 5;
  }
  return std::strtoul(str, NULL, 0);
}

bool has_key(std::map<std::string, std::string>& map, const std::string key) {
  return map.count(key);
}
}
