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

std::string get_or_die(const char* key) {
  const char* str = std::getenv(key);
  if (!str) {
    throw std::runtime_error(std::string(key) + " is unset");
  }
  return str;
}

std::string host() {
  return get_or_die("JUBATUS_HOST");
}

unsigned int port() {
  const char* v = get_or_die("JUBATUS_PORT").c_str();
  return std::strtoul(v, NULL, 0);
}

std::string cluster_name() {
  return get_or_die("JUBATUS_CLUSTER_NAME");
}

unsigned int timeout() {
  return std::strtoul(get_or_die("JUBATUS_TIMEOUT").c_str(), NULL, 0);
}

unsigned int servers_count() {
  return std::strtoul(get_or_die("JUBATUS_SERVERS_COUNT").c_str(), NULL, 0);
}

bool is_standalone() {
  return cluster_name().length() == 0;
}

bool has_key(std::map<std::string, std::string>& map, const std::string key) {
  return map.count(key);
}

}  // namespace
