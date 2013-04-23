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

#include "cached_zk.hpp"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <pficommon/concurrent/lock.h>

using pfi::concurrent::scoped_lock;
using std::vector;
using std::string;

namespace jubatus {
namespace server {
namespace common {

cached_zk::cached_zk(const string& hosts, int timeout, const string& logfile)
    : zk(hosts, timeout, logfile) {
}

cached_zk::~cached_zk() {
}

bool cached_zk::list(const string& path, vector<string>& out) {
  out.clear();
  scoped_lock lk(m_);
  std::map<string, std::set<string> >::const_iterator it = list_cache_.find(
      path);
  if (it == list_cache_.end()) {
    // First time, get list to create cache and set the watcher
    {
      DLOG(INFO) << "creating cache: " << path;
    }
    std::set<string> tmp_list;
    list_(path, tmp_list);
    list_cache_[path] = tmp_list;

    for (std::set<string>::const_iterator i = tmp_list.begin();
        i != tmp_list.end(); ++i) {
      out.push_back(*i);
    }

  } else {
    for (std::set<string>::const_iterator i = it->second.begin();
        i != it->second.end(); ++i) {
      out.push_back(*i);
    }
  }

  return true;
}

bool cached_zk::list_(const string& path, std::set<std::string>& out) {
  out.clear();
  struct String_vector s;
  int rc = zoo_wget_children(zh_, path.c_str(), cached_zk::update_cache, this,
                             &s);

  if (rc == ZOK) {
    for (int i = 0; i < s.count; ++i) {
      out.insert(s.data[i]);
    }
    return true;
  } else {
    LOG(ERROR) << "failed to get children: " << path << " - " << zerror(rc);
    return false;
  }
}

bool cached_zk::hd_list(const string& path, string& out) {
  out.clear();
  scoped_lock lk(m_);
  const std::set<string>& list(list_cache_[path]);
  if (!list.empty()) {
    out = *(list.begin());
  }

  return true;
}

const string cached_zk::type() const {
  return "cached_zk";
}

void cached_zk::reload_cache(const string& path) {
  scoped_lock lk(m_);
  list_(path, list_cache_[path]);
}

void cached_zk::clear_cache(const char* path) {
  scoped_lock lk(m_);
  if (path == NULL) {
    list_cache_.clear();
    znode_cache_.clear();
  } else {
    list_cache_.erase(string(path));
    znode_cache_.erase(string(path));
  }
}

bool cached_zk::read(const string& path, string& out) {
  scoped_lock lk(m_);
  std::map<string, string>::const_iterator it = znode_cache_.find(path);

  if (it == znode_cache_.end()) {
    {
      DLOG(INFO) << "creating cache: " << path;
    }

    if (read_(path, out)) {
      znode_cache_[path] = out;
    }

  } else {
    out = it->second;
  }
  return true;
}

bool cached_zk::read_(const string& path, string& out) {
  char buf[1024];
  int buflen = 1024;
  int rc = zoo_wget(zh_, path.c_str(), cached_zk::update_cache, this, buf,
                    &buflen, NULL);

  if (rc == ZOK) {
    out = string(buf, buflen);
    return buflen <= 1024;
  } else {
    LOG(ERROR) << zerror(rc);
    return false;
  }
}

void cached_zk::update_cache(
    zhandle_t* zh,
    int type,
    int state,
    const char* path,
    void* ctx) {
  cached_zk* zk = static_cast<cached_zk*>(ctx);

  if (type == ZOO_CHILD_EVENT) {
    // update cache

    {
      LOG(INFO) << "ZOO_CHILD_EVENT: " << path;
    }
    zk->reload_cache(path);

  } else if (type == ZOO_DELETED_EVENT ||    // clear one cache
      type == ZOO_NOTWATCHING_EVENT ||  // clear one cache?
      type == ZOO_SESSION_EVENT) {  // path == NULL, clear all cache
      // clear cache
    {
      LOG(INFO) << "ZOO_(DELETED|NOTWATCHING|SESSION)_EVENT: " << path;
    }
    zk->clear_cache(path);
  }
  // ZOO_CREATED_EVENT => ignore
  // ZOO_CHANGED_EVENT
  //  => ignore
  // TODO(kashihara): when read() cache going to be modified this needs fix
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
