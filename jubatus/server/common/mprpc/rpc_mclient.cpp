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

#include "rpc_mclient.hpp"
#include <string>
#include <utility>
#include "jubatus/server/common/logger/logger.hpp"

#include "jubatus/util/system/syscall.h"

namespace jubatus {
namespace server {
namespace common {
namespace mprpc {

rpc_result_object rpc_mclient::wait(const std::string& method) {
  rpc_result_object result;

  if (hosts_.empty()) {
    throw JUBATUS_EXCEPTION(rpc_no_client() << error_method(method));
  }

  for (size_t i = 0; i < futures_.size(); ++i) {
    try {
      result.response.push_back(wait_one(method, futures_[i]));
      result.error.push_back(
          rpc_error(hosts_[i].first,
                    hosts_[i].second));
    } catch(...) {
      // store exception_thrower to list of error
      result.error.push_back(
          rpc_error(hosts_[i].first,
                    hosts_[i].second,
                    jubatus::core::common::exception::get_current_exception()));
    }
  }

  if (result.response.empty()) {
    rpc_no_result e;
    if (result.has_error()) {
      e << error_multi_rpc(result.error);
    }
    throw JUBATUS_EXCEPTION(e << error_method(method));
  }

  return result;
}

rpc_response_t rpc_mclient::wait_one(
    const std::string& method,
    msgpack::rpc::future& f) {
  try {
    f.join();
    rpc_response_t rpc_resp(f);
    return rpc_resp;
  }
  JUBATUS_MSGPACKRPC_EXCEPTION_DEFAULT_HANDLER(method);
}

std::string create_error_string(const msgpack::object& error) {
  switch (error.type) {
    case msgpack::type::RAW:
      return error.as<std::string>();

    case msgpack::type::POSITIVE_INTEGER:
      switch (error.as<unsigned int>()) {
        case msgpack::rpc::NO_METHOD_ERROR:
          return "no method error";
        case msgpack::rpc::ARGUMENT_ERROR:
          return "argument error";
        default:
          {
            std::string msg = "unknown remote error (";
            msg += jubatus::util::lang::lexical_cast<std::string>(
                error.as<unsigned int>());
            msg += ")";
            return msg;
          }
      }

    case msgpack::type::NEGATIVE_INTEGER:
      // local errno(system error) carried as negative_integer
      {
        const int error_code = -error.as<int>();
        std::string msg("system error: ");
        msg += jubatus::util::system::syscall::get_error_msg(error_code);
        msg += " (" +
          jubatus::util::lang::lexical_cast<std::string>(error_code) + ")";
        return msg;
      }

    default:
      return "unknown error";
  }
}

}  // namespace mprpc
}  // namespace common
}  // namespace server
}  // namespace jubatus
