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

#include "cht.hpp"

#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <glog/logging.h>
#include <pficommon/data/digest/md5.h>
#include <pficommon/lang/shared_ptr.h>
#include "jubatus/core/common/exception.hpp"
#include "membership.hpp"


namespace jubatus {
namespace server {
namespace common {

std::string make_hash(const std::string& key) {
  std::stringstream ss;
  ss << pfi::data::digest::md5sum(key);
  return ss.str();
}

void cht::setup_cht_dir(
    lock_service& ls,
    const std::string& type,
    const std::string& name) {
  bool success = true;

  std::string path;
  build_actor_path(path, type, name);
  success = ls.create(path) && success;

  path += "/cht";
  success = ls.create(path) && success;

  if (!success) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("Failed to create cht directory")
        << jubatus::exception::error_api_func("lock_service::create")
        << jubatus::exception::error_message("cht path: " + path));
  }
}

cht::cht(
    pfi::lang::shared_ptr<lock_service> z,
    const std::string& type,
    const std::string& name)
    : type_(type),
      name_(name),
      lock_service_(z) {
}

cht::~cht() {
}

// register_node :: node -> bool;
// creates /jubatus/actors/<name>/cht/<hash(ip_port)> with contents ip_port
void cht::register_node(const std::string& ip, int port) {
  std::string path;
  build_actor_path(path, type_, name_);
  path += "/cht";

  for (unsigned int i = 0; i < NUM_VSERV; ++i) {
    std::string hashpath = path + "/" + make_hash(build_loc_str(ip, port, i));
    if (!lock_service_->create(hashpath, build_loc_str(ip, port), true)) {
      throw JUBATUS_EXCEPTION(
          jubatus::exception::runtime_error("Failed to register cht node")
          << jubatus::exception::error_api_func("lock_service::create")
          << jubatus::exception::error_message("cht hashpash: " + hashpath));
    }

    DLOG(INFO) << "cht node created: " << hashpath;
  }
}

bool cht::find(
    const std::string& host,
    int port,
    std::vector<std::pair<std::string, int> >& out,
    size_t s) {
  return find(build_loc_str(host, port), out, s);
}

// return at most n nodes, if theres nodes less than n, return size is also less
// than n.
// find(hash) :: lock_service -> key -> [node]
//   where hash(node0) <= hash(key) < hash(node1)
bool cht::find(
    const std::string& key,
    std::vector<std::pair<std::string, int> >& out,
    size_t n) {
  out.clear();
  std::vector<std::string> hlist;
  if (!get_hashlist_(key, hlist)) {
    throw JUBATUS_EXCEPTION(not_found(key));
  }
  std::string hash = make_hash(key);
  std::string path;
  build_actor_path(path, type_, name_);
  path += "/cht";

  std::vector<std::string>::iterator node0 = std::lower_bound(hlist.begin(),
                                                              hlist.end(),
                                                              hash);

  size_t idx = static_cast<int>(node0 - hlist.begin()) % hlist.size();
  std::string loc;
  for (size_t i = 0; i < n; ++i) {
    std::string ip;
    int port;
    if (lock_service_->read(path + "/" + hlist[idx], loc)) {
      revert(loc, ip, port);
      out.push_back(make_pair(ip, port));
    } else {
      // TODO(kuenishi): output log
      throw JUBATUS_EXCEPTION(not_found(path));
    }
    idx++;
    idx %= hlist.size();
  }
  return !hlist.size();
}

std::pair<std::string, int> cht::find_predecessor(
    const std::string& host,
    int port) {
  return find_predecessor(build_loc_str(host, port));
}

std::pair<std::string, int> cht::find_predecessor(const std::string& key) {
  std::vector<std::string> hlist;
  get_hashlist_(key, hlist);

  std::string hash = make_hash(key);
  std::string path;
  build_actor_path(path, type_, name_);
  path += "/cht";

  std::vector<std::string>::iterator node0 = std::lower_bound(hlist.begin(),
                                                              hlist.end(),
                                                              hash);

  size_t idx = (static_cast<int>(node0 - hlist.begin()) + hlist.size() - 1)
    % hlist.size();

  std::string ip;
  int port;
  std::string loc;
  if (lock_service_->read(path + "/" + hlist[idx], loc)) {
    revert(loc, ip, port);
    return make_pair(ip, port);
  } else {
    throw JUBATUS_EXCEPTION(not_found(path));
    // TODO(kuenishi): output log and throw exception
  }
}

bool cht::get_hashlist_(
    const std::string& key,
    std::vector<std::string>& hlist) {
  hlist.clear();
  std::string path;
  build_actor_path(path, type_, name_);
  path += "/cht";

  if (!lock_service_->list(path, hlist) || hlist.empty()) {
    return false;
  }

  std::sort(hlist.begin(), hlist.end());
  return true;
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
