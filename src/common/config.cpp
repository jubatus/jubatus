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
  bool success = true;
  string path;
  build_config_path(path, type, name);
  success = z.exists(path) && success;

//  success = z.create(path + "/config_lock", "") && success;
  common::lock_service_mutex zlk(z, path);
  while(!zlk.try_lock()){ ; }

  success = z.read(path, config) && success;

  if (!success)
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("Failed to get config from zookeeper")
        << jubatus::exception::error_api_func("lock_service::create"));
}

void config_tozk(lock_service& z,
                    const string& type, const string& name,
                    string& config)
{
  bool success = true;
  string path;
  build_config_path(path, type, name);

//  success = z.create(path + "/config_lock", "") && success;
  common::lock_service_mutex zlk(z, path);
  while(!zlk.try_lock()){ ; }

  success = z.create(path, config) && success;
  
  if (!success)
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("Failed to set config to zookeeper")
        << jubatus::exception::error_api_func("lock_service::create"));
}
#endif


} // common
} // jubatus
