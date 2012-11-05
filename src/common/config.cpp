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

#include "config.hpp"

#include <cstdlib>
#include <iostream>
#include <pficommon/lang/cast.h>

using namespace std;
using namespace pfi::lang;

namespace jubatus {
namespace common {


bool getconfig_fromzk(lock_service& z,
                    const string& type, const string& name,
                    string& config)
{
  string path;
  build_config_path(path, type, name);
  if (!z.exists(path)){
    return false;
  }

  z.create(path + "/config_lock", "");
  common::lock_service_mutex zlk(z, path + "/config_lock");
  while(!zlk.try_lock()){ ; }

  z.read(path, config);
  return true;
}

bool setconfig_tozk(lock_service& z,
                    const string& type, const string& name,
                    string& config)
{
  string path;
  build_config_path(path, type, name);

  z.create(path + "/config_lock", "");
  common::lock_service_mutex zlk(z, path + "/config_lock");
  while(zlk.try_lock()){ ; }

  z.create(path, config);
  return true;
}



} // common
} // jubatus
