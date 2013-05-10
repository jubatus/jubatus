// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_NEAREST_NEIGHBOR_EXCEPTION_HPP_
#define JUBATUS_NEAREST_NEIGHBOR_EXCEPTION_HPP_

#include <string>
#include "../common/exception.hpp"

namespace jubatus {
namespace nearest_neighbor {

class nearest_neighbor_exception
    : public exception::jubaexception<nearest_neighbor_exception> {
 public:
  nearest_neighbor_exception(const std::string& msg)
      : msg(msg) {}
  ~nearest_neighbor_exception() throw() {}
  const char* what() const throw() {
    return msg.c_str();
  }

 private:
  std::string msg;
};

class unimplemented_exception : public nearest_neighbor_exception {
 public:
  unimplemented_exception(const std::string& msg)
      : nearest_neighbor_exception(msg) {}
};

}  // namespace nearest_neighbor
}  // namespace jubatus

#endif  // JUBATUS_NEAREST_NEIGHBOR_EXCEPTION_HPP_
