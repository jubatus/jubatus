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
#include "exception.hpp"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <pficommon/lang/cast.h>

using namespace std;
using namespace pfi::lang;
using namespace pfi::text::json;

namespace jubatus {
namespace common {


void config_fromlocal(const string& path, string& config)
{
  ifstream ifc(path.c_str());
  if (!ifc){
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("can't read " + path + " ."));
  }
  stringstream ss;
  ss << ifc.rdbuf();
  config = ss.str();
}

#ifdef HAVE_ZOOKEEPER_H
void config_fromzk(lock_service& z,
                    const string& type, const string& name,
                    string& config)
{
  string path;
  build_config_path(path, type, name);

  if(!z.exists(path))
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("config is not exists: " + path));

  common::lock_service_mutex zlk(z, path);
  while(!zlk.try_lock()){ ; }

  if (!z.read(path, config))
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("failed to get config from zookeeper: " + path)
        << jubatus::exception::error_api_func("lock_service::read"));

  LOG(INFO) << "get config from zookeeper: " << path;
  DLOG(INFO) << "config: " << config;
}

void config_tozk(lock_service& z,
                    const string& type, const string& name,
                    string& config)
{
  string path;
  build_config_path(path, type, name);

  if(!z.exists(path))
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("node is not exists: " + path));

  common::lock_service_mutex zlk(z, path);
  while(!zlk.try_lock()){ ; }

  if (!z.set(path, config))
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("failed to set config to zookeeper:" + path)
        << jubatus::exception::error_api_func("lock_service::set"));

  LOG(INFO) << "set config to zookeeper: " << path;
  DLOG(INFO) << "config: " << config;
}

void remove_config_fromzk(lock_service& z,
                          const string& type, const string& name)
{
  string path;
  build_config_path(path, type, name);

  if(!z.exists(path))
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("config is not exists: " + path));

  if (!z.remove(path))
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("failed to remove config from zookeeper:" + path)
        << jubatus::exception::error_api_func("lock_service::remove"));

  LOG(INFO) << "remove config from zookeeper: " << path;
}
#endif


} // common
} // jubatus
