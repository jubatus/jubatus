// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include "dynamic_loader.hpp"

#include <dlfcn.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "jubatus/core/fv_converter/exception.hpp"
#include "../common/filesystem.hpp"
#include "../common/logger/logger.hpp"

using jubatus::core::fv_converter::converter_exception;

namespace {
bool is_absolute_or_relative_path(const std::string& path) {
  return path.find('/') != std::string::npos;
}

const char* get_plugin_path() {
  return ::getenv("JUBATUS_PLUGIN_PATH");
}

}  // namespace

namespace jubatus {
namespace server {
namespace fv_converter {

dynamic_loader::dynamic_loader(const std::string& path)
    : handle_(0) {
  void* handle = NULL;
  std::string loaded_path;
  if (is_absolute_or_relative_path(path)) {
    // Load the plugin with the given path
    handle = ::dlopen(path.c_str(), RTLD_LAZY);
    loaded_path = path;

  } else {
    // Try to load the plugin from the plugin path environment
    const char* plugin_dir = get_plugin_path();
    if (plugin_dir) {
      const std::string plugin_path =
          std::string(plugin_dir) + "/" + path;
      handle = ::dlopen(plugin_path.c_str(), RTLD_LAZY);
      loaded_path = plugin_path;
    }

    // If failed, try to load it from the plugin directory specified on
    // configure.
    if (!handle) {
      const std::string plugin_path =
          std::string(JUBATUS_PLUGIN_DIR) + "/" + path;
      handle = ::dlopen(plugin_path.c_str(), RTLD_LAZY);
      loaded_path = plugin_path;
    }
  }

  handle_ = handle;

  if (!handle_) {
    char* error = dlerror();
    throw JUBATUS_EXCEPTION(
        converter_exception(
            "cannot load dynamic library: " + path + ": " + error)
        << jubatus::core::common::exception::error_api_func("dlopen")
        << jubatus::core::common::exception::error_file_name(path)
        << jubatus::core::common::exception::error_message(error));
  } else {
    try {
      typedef std::string (*func_t)(void);
      func_t version = reinterpret_cast<func_t>(load_symbol("version"));
      LOG(INFO) << "plugin loaded: " << common::real_path(loaded_path)
                << " version: " << version();
    } catch (converter_exception) {
      LOG(WARN) << "plugin loaded: " << common::real_path(loaded_path)
                << " but version information is unavailable";
    }
  }
}

dynamic_loader::~dynamic_loader() {
  if (handle_ && dlclose(handle_) != 0) {
    // TODO(kuenishi) failed
  }
}

void* dynamic_loader::load_symbol(const std::string& name) const {
  dlerror();
  void* func = dlsym(handle_, name.c_str());
  char* error = dlerror();
  if (error != NULL) {
    throw JUBATUS_EXCEPTION(converter_exception("cannot dlsym: " + name)
      << jubatus::core::common::exception::error_api_func("dlsym")
      << jubatus::core::common::exception::error_message("dlsym name: " + name)
      << jubatus::core::common::exception::error_message(error));
  }
  return func;
}

void check_null_instance(void* inst) {
  if (!inst) {
    throw JUBATUS_EXCEPTION(converter_exception("failed to load plugin"));
  }
}

}  // namespace fv_converter
}  // namespace server
}  // namespace jubatus
