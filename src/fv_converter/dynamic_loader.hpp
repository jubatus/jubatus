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

#pragma once

#include <string>
#include <map>

namespace jubatus {

class dynamic_loader {
 public:
  dynamic_loader(const std::string& path);
  ~dynamic_loader();

  void *load_symbol(const std::string& name) const;

 private:
  void* handle_;
};

template<typename T>
T* load_object(const dynamic_loader& loader,
               const std::string& function,
               const std::map<std::string, std::string>& params) {
  typedef T* (*func_t)(const std::map<std::string, std::string>&);
  func_t func = (func_t)loader.load_symbol(function);
  return (*func)(params);
}



}
