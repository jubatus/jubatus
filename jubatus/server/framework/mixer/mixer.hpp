// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011-2013 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_FRAMEWORK_MIXER_MIXER_HPP_
#define JUBATUS_SERVER_FRAMEWORK_MIXER_MIXER_HPP_

#include <string>

#include "jubatus/util/lang/noncopyable.h"
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/core/driver/driver.hpp"
#include "../server_base.hpp"
#include "../../common/mprpc/rpc_server.hpp"

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {

class mixer : util::lang::noncopyable {
 public:
  typedef jubatus::server::common::mprpc::rpc_server rpc_server_t;

  virtual ~mixer() {
  }

  virtual void register_api(rpc_server_t& server) = 0;
  virtual void set_driver(core::driver::driver_base*) = 0;

  virtual void start() = 0;
  virtual void stop() = 0;

  virtual void updated() = 0;

  virtual void get_status(server_base::status_t& status) const = 0;

  virtual std::string type() const = 0;
};

class unsupported_mixables : public core::common::exception::runtime_error {
 public:
  explicit unsupported_mixables(const std::string& type)
      : jubatus::core::common::exception::runtime_error(
          std::string(type + " does not support this configuration")) {
  }
};

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_MIXER_MIXER_HPP_
