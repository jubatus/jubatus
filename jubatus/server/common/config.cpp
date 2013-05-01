// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <pficommon/lang/cast.h>
#include <pficommon/text/json.h>
#include "jubatus/core/common/exception.hpp"

using std::ifstream;
using std::string;
using std::stringstream;

namespace jubatus {
namespace server {
namespace common {

void config_fromlocal(const string& path, string& config) {
  ifstream ifc(path.c_str());
  if (!ifc) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("can't read " + path + " ."));
  }
  stringstream ss;
  ss << ifc.rdbuf();
  config = ss.str();
}

#ifdef HAVE_ZOOKEEPER_H
void config_fromzk(
    lock_service& z,
    const string& type,
    const string& name,
    string& config) {
  // server must get read lock for config_lock before call this method.
  string path;
  build_config_path(path, type, name);

  if (!z.exists(path)) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("config is not exists: " + path));
  }

  if (!z.read(path, config)) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
          "failed to get config from zookeeper: " + path)
        << jubatus::exception::error_api_func("lock_service::read"));
  }

  LOG(INFO) << "get config from zookeeper: " << path;
}

void config_tozk(
    lock_service& z,
    const string& type,
    const string& name,
    string& config) {
  config_tozk(z, type, name, config, string("<istream>"));
}

void config_tozk(
    lock_service& z,
    const string& type,
    const string& name,
    string& config,
    const string& config_src) {
  if (config == "") {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("config is empty"));
  }

  try {
    pfi::lang::lexical_cast<pfi::text::json::json>(config);
  } catch (pfi::lang::parse_error& e) {
    std::string msg =
        std::string("syntax error in configuration: ") +
        config_src + ":" +
        pfi::lang::lexical_cast<std::string>(e.lineno()) + ":" +
        pfi::lang::lexical_cast<std::string>(e.pos()) + " " +
        e.msg();
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(msg));
  }

  string lock_path;
  build_config_lock_path(lock_path, type, name);

  if (!z.exists(lock_path)) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("node is not exists: " + lock_path));
  }

  common::lock_service_mutex zk_config_lock(z, lock_path);
  int retry = 3;
  while (!zk_config_lock.try_lock()) {
    if (retry == 0) {
      throw JUBATUS_EXCEPTION(
          jubatus::exception::runtime_error("any user is using config?"));
    }
    retry--;
    sleep(1);
  }

  if (!is_no_workers(z, type, name)) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
          "any server is running: " + type + ", " + name));
  }

  string path;
  build_config_path(path, type, name);

  bool success = true;
  success = z.create(path) && success;
  success = z.set(path, config) && success;

  if (!success) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
          "failed to set config to zookeeper:" + path)
        << jubatus::exception::error_api_func("lock_service::set"));
  }

  LOG(INFO) << "set config to zookeeper: " << path;
}

void remove_config_fromzk(
    lock_service& z,
    const string& type,
    const string& name) {
  string lock_path;
  build_config_lock_path(lock_path, type, name);

  if (!z.exists(lock_path)) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("node is not exists: " + lock_path));
  }

  common::lock_service_mutex zk_config_lock(z, lock_path);
  int retry = 3;
  while (!zk_config_lock.try_lock()) {
    if (retry == 0) {
      throw JUBATUS_EXCEPTION(
          jubatus::exception::runtime_error("any user is using config?"));
    }
    retry--;
    sleep(1);
  }

  if (!is_no_workers(z, type, name)) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
          "any server is running: " + type + ", " + name));
  }

  string path;
  build_config_path(path, type, name);

  if (!z.exists(path)) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("config is not exists: " + path));
  }

  if (!z.remove(path)) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
          "failed to remove config from zookeeper:" + path)
        << jubatus::exception::error_api_func("lock_service::remove"));
  }

  LOG(INFO) << "remove config from zookeeper: " << path;
}

bool is_no_workers(lock_service& z, const string& type, const string& name) {
  std::vector<std::pair<std::string, int> > nodes;
  get_all_actors(z, type, name, nodes);
  if (nodes.empty()) {
    return true;
  } else {
    return false;
  }
}

#endif

}  // namespace common
}  // namespace server
}  // namespace jubatus
