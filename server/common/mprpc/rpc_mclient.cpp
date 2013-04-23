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
#include <glog/logging.h>

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
    } catch(...) {
      // store exception_thrower to list of error
      result.error.push_back(
          rpc_error(hosts_[i].first,
                    hosts_[i].second,
                    jubatus::exception::get_current_exception()));
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

}  // namespace mprpc
}  // namespace common
}  // namespace server
}  // namespace jubatus
