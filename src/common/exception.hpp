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

#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace jubatus{

  class storage_not_set : public std::exception {};

  class config_not_set : public std::runtime_error {
  public:
    config_not_set(): runtime_error("config_not_set") {}
  };

  class unsupported_method : public std::runtime_error {
  public:
    unsupported_method(const std::string& n): runtime_error(n) {}
  };

  class bad_storage_type : public std::runtime_error {
  public:
    bad_storage_type(const std::string& n):runtime_error(n){};
  };

  class membership_error : public std::runtime_error {
  public:
    membership_error(const std::string& n):runtime_error(n){};
  };

  class argv_error : public std::runtime_error {
  public:
    argv_error(const std::string& n):runtime_error(n){};
  };

}
