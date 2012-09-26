// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
#include <pficommon/lang/cast.h>

using namespace std;
using namespace pfi::lang;

namespace jubatus {
namespace common {

// "127.0.0.1" -> 9199 -> "127.0.0.1_9199"
string build_loc_str(const string& ip, int port, unsigned int i)
{
  string ret = ip + "_" + lexical_cast<string, int>(port);
  if (i > 0) {
    ret += "_";
    ret += lexical_cast<string,int>(i);
  }
  return ret;
}

// /path/base -> 127.0.0.1 -> 9199 -> /path/base/127.0.0.1_9199
void build_existence_path(const string& base, const string& ip, int port, string& out)
{
  out = base + "/" + ip + "_" + lexical_cast<string, int>(port);
}

void build_actor_path(string& path, const string& type, const string& name)
{
  path = ACTOR_BASE_PATH + "/" + type + "/" + name;
}

// 127.0.0.1_9199 -> (127.0.0.1, 9199)
bool revert(const string& name, string& ip, int& port)
{
  ip = name.substr(0, name.find("_"));
  port = atoi(name.substr(name.find("_") + 1).c_str());
  return true;
}

// zk -> name -> ip -> port -> bool
bool register_actor(lock_service& z,
                    const string& type, const string& name,
                    const string& ip, int port)
{
  string path;
  build_actor_path(path, type, name);
  z.create(path, "");
  z.create(path + "/master_lock", "");
  path += "/nodes";
  z.create(path , "");
  {
    string path1;
    build_existence_path(path, ip, port, path1);
    z.create(path1, "", true);
  }

  // set exit zlistener here
  pfi::lang::function <void()> f = &force_exit;
  z.push_cleanup(f);

  return true;
}

bool register_keeper(lock_service& z, const string& type, const string& ip, int port)
{
  string path = JUBAKEEPER_BASE_PATH;
  z.create(path, "");
  path += "/" + type;
  z.create(path, "");
  {
    string path1;
    build_existence_path(path, ip, port, path1);
    z.create(path1, "", true);
  }
  // set exit zlistener here
  pfi::lang::function <void()> f = &force_exit;
  z.push_cleanup(f);
  return true;
}

// zk -> name -> list( (ip, rpc_port) )
bool get_all_actors(lock_service& z,
                    const string& type, const string& name,
                    std::vector<std::pair<string, int> >& ret)
{
  ret.clear();
  string path;
  build_actor_path(path, type, name);
  path += "/nodes";
  std::vector<string> list;
  z.list(path, list);
  for (std::vector<string>::const_iterator it = list.begin();
      it != list.end(); ++it) {
    string ip;
    int port;
    revert(*it, ip, port);
    ret.push_back(make_pair(ip,port));
  }
  return true;
}

bool push_cleanup(lock_service& z, pfi::lang::function<void()>& f)
{
  z.push_cleanup(f);
  return true;
}

void force_exit()
{
  exit(-1);
}

void prepare_jubatus(lock_service& ls, const string& type, const string& name)
{
  ls.create(JUBATUS_BASE_PATH);
  ls.create(JUBAVISOR_BASE_PATH);
  ls.create(JUBAKEEPER_BASE_PATH);
  ls.create(ACTOR_BASE_PATH);

  string path = ACTOR_BASE_PATH + "/" + type;
  ls.create(path);
  if (name != "") {
    build_actor_path(path, type, name);
    ls.create(path);
  }
}

} // common
} // jubatus
