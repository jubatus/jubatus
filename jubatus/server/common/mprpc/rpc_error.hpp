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

#ifndef JUBATUS_SERVER_COMMON_MPRPC_RPC_ERROR_HPP_
#define JUBATUS_SERVER_COMMON_MPRPC_RPC_ERROR_HPP_

#include <string>
#include <vector>

#include <pficommon/data/string/utility.h>

#include "jubatus/core/common/exception.hpp"
#include "exception.hpp"

namespace jubatus {
namespace server {
namespace common {
namespace mprpc {

// class rpc_mclient;

class rpc_error {
 public:
  rpc_error(const std::string& host, uint16_t port)
      : host_(host),
        port_(port) {
  }

  rpc_error(const std::string& host, uint16_t port,
      jubatus::exception::exception_thrower_ptr thrower)
      : host_(host),
        port_(port),
        exception_(thrower) {
  }

  std::string host() const {
    return host_;
  }
  uint16_t port() const {
    return port_;
  }

  bool has_exception() const {
    return exception_;
  }
  void throw_exception() const {
    exception_->throw_exception();
  }
  jubatus::exception::exception_thrower_ptr exception() const {
    return exception_;
  }

 private:
  std::string host_;
  uint16_t port_;
  jubatus::exception::exception_thrower_ptr exception_;
};

typedef jubatus::exception::error_info<
    struct error_multi_rpc_,
    std::vector<rpc_error> > error_multi_rpc;

inline std::string to_string(const error_multi_rpc& info) {
  std::ostringstream result;

  size_t host_size = info.value().size();
  if (host_size == 1) {
    result << "rpc_error with 1 server" << '\n';
  } else if (host_size > 1) {
    result << "rpc_error with " << host_size << " servers" << '\n';
  }

  std::vector<rpc_error> errors = info.value();
  for (std::vector<rpc_error>::const_iterator
      it = errors.begin(), end = errors.end();
      it != end; ++it) {
    result << " host: " << it->host() << ", port: " << it->port() << '\n';
    std::ostringstream tmp;
    if (it->has_exception()) {
      try {
        it->throw_exception();
      } catch (const jubatus::exception::jubatus_exception& e) {
        tmp << e.diagnostic_information(false);
      } catch (const std::exception& e) {
        tmp << e.what();
      } catch (...) {
        tmp << "...";
      }

      // Indent each line
      std::vector<std::string> lines =
          pfi::data::string::split(tmp.str(), '\n');
      std::ostringstream msg;
      for (std::vector<std::string>::iterator
          it = lines.begin(), end = lines.end();
          it != end; ++it) {
        if (it->empty()) {
          continue;
        }
        msg << "   " << *it << '\n';
      }
      result << msg.str();
    }
  }

  return result.str();
}

}  // namespace mprpc
}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_MPRPC_RPC_ERROR_HPP_
