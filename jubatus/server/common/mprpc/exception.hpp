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

#ifndef JUBATUS_SERVER_COMMON_MPRPC_EXCEPTION_HPP_
#define JUBATUS_SERVER_COMMON_MPRPC_EXCEPTION_HPP_

#include <string>
#include "jubatus/core/common/exception.hpp"

namespace jubatus {
namespace server {
namespace common {
namespace mprpc {

typedef jubatus::exception::error_info<
    struct error_method_,
    std::string> error_method;

class rpc_no_client : public jubatus::exception::jubaexception<rpc_no_client> {
 public:
  const char* what() const throw() {
    return "no client";
  }
};

class rpc_no_result : public jubatus::exception::jubaexception<rpc_no_result> {
 public:
  const char* what() const throw() {
    return "no result";
  }
};

class rpc_io_error : public jubatus::exception::jubaexception<rpc_io_error> {
};

class rpc_timeout_error
    : public jubatus::exception::jubaexception<rpc_timeout_error> {
};

// rpc_server error
class rpc_type_error
    : public jubatus::exception::jubaexception<rpc_type_error> {
};

// rpc_server error
class rpc_method_not_found
    : public jubatus::exception::jubaexception<rpc_method_not_found> {
};

// rpc_server error
class rpc_call_error
    : public jubatus::exception::jubaexception<rpc_call_error> {
};

class rpc_internal_error
    : public jubatus::exception::jubaexception<rpc_internal_error> {
};

}  // namespace mprpc
}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_MPRPC_EXCEPTION_HPP_
