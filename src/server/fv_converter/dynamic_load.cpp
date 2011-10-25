// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include <string>
#include <dlfcn.h>
#include "exception.hpp"

using namespace std;

namespace jubatus {

void* dynamic_load(const string& path,
                   const string& function) {
  void* handle = dlopen(path.c_str(), RTLD_LAZY);
  if (!handle) {
    char *error = dlerror();
    throw converter_exception("cannot load dynamic library: " + path + ": "
                              + error);
  }

  dlerror();
  void* func = dlsym(handle, function.c_str());
  char* error = dlerror();
  if (error != NULL) {
    dlclose(handle);
    throw converter_exception(error);
  }
  return func;
}

}
