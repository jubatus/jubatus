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

#include <dlfcn.h>
#include "exception.hpp"
#include "dynamic_loader.hpp"
#include <iostream>

namespace jubatus {
namespace fv_converter {

dynamic_loader::dynamic_loader(const std::string& path)
    : handle_(0) {
  void* handle = dlopen(path.c_str(), RTLD_LAZY);
  if (!handle) {
    char *error = dlerror();
    throw JUBATUS_EXCEPTION(converter_exception("cannot load dynamic library: " + path + ": "
                              + error)
        << jubatus::exception::error_api_func("dlopen")
        << jubatus::exception::error_file_name(path)
        << jubatus::exception::error_message(error));
  }
  handle_ = handle;
}

dynamic_loader::~dynamic_loader() {
  if (handle_ && dlclose(handle_) != 0) {
    // TODO failed
  }
}

void* dynamic_loader::load_symbol(const std::string& name) const {
  dlerror();
  void* func = dlsym(handle_, name.c_str());
  char* error = dlerror();
  if (error != NULL) {
    throw JUBATUS_EXCEPTION(converter_exception("cannot dlsym: " + name)
        << jubatus::exception::error_api_func("dlsym")
        << jubatus::exception::error_message("dlsym name: " + name)
        << jubatus::exception::error_message(error));
  }
  return func;
}

}
}
