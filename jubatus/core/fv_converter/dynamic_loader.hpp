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

#ifndef JUBATUS_CORE_FV_CONVERTER_DYNAMIC_LOADER_HPP_
#define JUBATUS_CORE_FV_CONVERTER_DYNAMIC_LOADER_HPP_

#include <map>
#include <string>

namespace jubatus {
namespace core {
namespace fv_converter {

class dynamic_loader {
 public:
  explicit dynamic_loader(const std::string& path);
  ~dynamic_loader();

  void* load_symbol(const std::string& name) const;

 private:
  void* handle_;
};

template<typename T>
T* load_object(
    const dynamic_loader& loader,
    const std::string& function,
    const std::map<std::string, std::string>& params) {
  typedef T* (*func_t)(const std::map<std::string, std::string>&);
  func_t func = reinterpret_cast<func_t>(loader.load_symbol(function));
  return (*func)(params);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FV_CONVERTER_DYNAMIC_LOADER_HPP_
