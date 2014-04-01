// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "cht_local.hpp"

#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <map>
#include <vector>
#include <glog/logging.h>
#include "jubatus/util/data/digest/md5.h"

#include "../common/exception.hpp"
#include "jubatus/server/common/membership.hpp"

using std::string;
using std::stringstream;
using std::map;
using std::vector;
using std::pair;

namespace jubatus {
namespace core {
namespace burst {
namespace storage {

string make_hash(const string& key) {
  stringstream ss;
  ss << jubatus::util::data::digest::md5sum(key);
  return ss.str();
}

cht_local::~cht_local() {
}

// register_node :: node -> bool;
// creates /jubatus/actors/<name>/cht/<hash(ip_port)> with contents ip_port
void cht_local::register_node(const string& ip, int port) {
  for (unsigned int i = 0; i < NUM_VSERV; ++i) {
    string build_loc = jubatus::server::common::build_loc_str(ip, port, i);
    string hash_key = make_hash(build_loc);
    string hash_value = jubatus::server::common::build_loc_str(ip, port);

    DLOG(INFO) << "cht created: "
               << build_loc
               << "->"
               << hash_key;
    hashmap_.insert(map<string, string>::value_type(hash_key, hash_value));

    DLOG(INFO) << "hashmap added: "
               << "key="
               << hash_key
               << ", value="
               << hash_value;
  }
}

void cht_local::remove_all_nodes() {
  hashmap_.clear();
}

bool cht_local::find(const string& host,
                     int port,
                     vector<pair<string, int> >& out,
                     size_t s) {
  return find(jubatus::server::common::build_loc_str(host, port), out, s);
}

// return at most n nodes
// if theres nodes less than n, return size is also less
// than n.
// find(hash) :: lock_service -> key -> [node]
//   where hash(node0) <= hash(key) < hash(node1)
bool cht_local::find(const string& key,
                     vector<pair<string, int> >& out,
                     size_t n) {
  DLOG(INFO) << "find start: key=" << key;
  out.clear();
  vector<string> hlist;
  if (!get_hashlist_(key, hlist)) {
    throw JUBATUS_EXCEPTION(core::common::not_found(key));
  }
  string hash = make_hash(key);
  vector<string>::iterator node0
      = std::lower_bound(hlist.begin(),
                         hlist.end(),
                         hash);

  size_t idx = static_cast<int>(node0 - hlist.begin()) % hlist.size();
  string loc;
  for (size_t i = 0; i < n; ++i) {
    string ip;
    int port;
    string loc = hashmap_[hlist[idx]];
    jubatus::server::common::revert(loc, ip, port);
    out.push_back(make_pair(ip, port));
    idx++;
    idx %= hlist.size();
  }
  return !hlist.size();
}


bool cht_local::get_hashlist_(const string& key,
                              vector<string>& hlist) {
  hlist.clear();
  if (hashmap_.size() == 0) {
    return false;
  }

  map<string, string>::iterator it;
  for (it = hashmap_.begin(); it != hashmap_.end(); it++) {
    hlist.push_back(it->first);
  }

  std::sort(hlist.begin(), hlist.end());
  return true;
}

void cht_local::show_hash() const {
  map<string, string>::const_iterator it;
  for (it = hashmap_.begin(); it != hashmap_.end(); it++) {
    std::cout << it->first
              << ","
              << it->second
              << std::endl;
  }
}

}  // namespace storage
}  // namespace burst
}  // namespace core
}  // namespace jubatus

