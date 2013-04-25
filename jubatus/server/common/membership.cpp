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

#include "membership.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <pficommon/lang/cast.h>
#include <glog/logging.h>
#include "jubatus/core/common/exception.hpp"

using std::string;
using std::vector;
using pfi::lang::lexical_cast;

namespace jubatus {
namespace server {
namespace common {

// "127.0.0.1" -> 9199 -> "127.0.0.1_9199"
string build_loc_str(const string& ip, int port, unsigned int i) {
  string ret = ip + "_" + lexical_cast<string, int>(port);
  if (i > 0) {
    ret += "_";
    ret += lexical_cast<string, int>(i);
  }
  return ret;
}

// /path/base -> 127.0.0.1 -> 9199 -> /path/base/127.0.0.1_9199
void build_existence_path(
    const string& base,
    const string& ip,
    int port,
    string& out) {
  out = base + "/" + ip + "_" + lexical_cast<string, int>(port);
}

void build_actor_path(string& path, const string& type, const string& name) {
  path = ACTOR_BASE_PATH + "/" + type + "/" + name;
}

void build_config_path(string& path, const string& type, const string& name) {
  path = CONFIG_BASE_PATH + "/" + type + "/" + name;
}

void build_config_lock_path(
    string& path,
    const string& type,
    const string& name) {
  build_actor_path(path, type, name);
  path += "/config_lock";
}

// 127.0.0.1_9199 -> (127.0.0.1, 9199)
bool revert(const string& name, string& ip, int& port) {
  ip = name.substr(0, name.find("_"));
  port = atoi(name.substr(name.find("_") + 1).c_str());
  return true;
}

// zk -> name -> ip -> port -> void
void register_actor(
    lock_service& z,
    const string& type,
    const string& name,
    const string& ip,
    int port) {
  bool success = true;

  string path;
  build_actor_path(path, type, name);
  success = z.create(path) && success;
  success = z.create(path + "/master_lock", "") && success;
  path += "/nodes";
  success = z.create(path) && success;
  {
    string path1;
    build_existence_path(path, ip, port, path1);
    success = z.create(path1, "", true) && success;
    if (success) {
      LOG(INFO) << "actor created: " << path1;
    }
  }

  if (!success) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("Failed to register_actor")
        << jubatus::exception::error_api_func("lock_service::create"));
  }

  // set exit zlistener here
  pfi::lang::function<void()> f = &force_exit;
  z.push_cleanup(f);
}

void register_keeper(
    lock_service& z,
    const string& type,
    const string& ip,
    int port) {
  bool success = true;

  string path = JUBAKEEPER_BASE_PATH;
  success = z.create(path) && success;
  path += "/" + type;
  success = z.create(path) && success;
  {
    string path1;
    build_existence_path(path, ip, port, path1);
    success = z.create(path1, "", true) && success;
    if (success) {
      LOG(INFO) << "keeper created: " << path1;
    }
  }

  if (!success) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("Failed to register_actor")
        << jubatus::exception::error_api_func("lock_service::create"));
  }

  // set exit zlistener here
  pfi::lang::function<void()> f = &force_exit;
  z.push_cleanup(f);
}

// zk -> name -> list( (ip, rpc_port) )
bool get_all_actors(
    lock_service& z,
    const string& type,
    const string& name,
    std::vector<std::pair<string, int> >& ret) {
  ret.clear();
  string path;
  build_actor_path(path, type, name);
  path += "/nodes";
  std::vector<string> list;
  if (!z.list(path, list)) {
    return false;
  }

  for (std::vector<string>::const_iterator it = list.begin(); it != list.end();
      ++it) {
    string ip;
    int port;
    revert(*it, ip, port);
    ret.push_back(make_pair(ip, port));
  }
  return true;
}

void force_exit() {
  exit(-1);
}

void prepare_jubatus(lock_service& ls, const string& type, const string& name) {
  bool success = true;
  success = ls.create(JUBATUS_BASE_PATH) && success;
  success = ls.create(JUBAVISOR_BASE_PATH) && success;
  success = ls.create(JUBAKEEPER_BASE_PATH) && success;
  success = ls.create(ACTOR_BASE_PATH) && success;
  success = ls.create(CONFIG_BASE_PATH) && success;

  string path = ACTOR_BASE_PATH + "/" + type;
  success = ls.create(path) && success;
  string cpath = CONFIG_BASE_PATH + "/" + type;
  success = ls.create(cpath) && success;

  if (name != "") {
    build_actor_path(path, type, name);
    build_config_lock_path(cpath, type, name);
    success = ls.create(path) && success;
    success = ls.create(cpath) && success;
  }

  if (!success) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("Failed to prepare lock_service")
        << jubatus::exception::error_api_func("lock_service::create"));
  }
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
