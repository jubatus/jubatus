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

#include "rpc_server.hpp"
#include <string>
#include <glog/logging.h>
#include "jubatus/core/common/exception.hpp"

namespace jubatus {
namespace server {
namespace common {
namespace mprpc {

// rpc_server
//   Msgpack-RPC based server with 'hashed' dispatcher.
//   rpc_server can add RPC method on-the-fly.
//
void rpc_server::dispatch(msgpack::rpc::request req) {
  std::string method;
  req.method().convert(&method);

  func_map::iterator fun = funcs_.find(method);
  if (fun == funcs_.end()) {
    req.error(msgpack::rpc::NO_METHOD_ERROR, method);
    return;
  }

  try {
    fun->second->invoke(req);
  } catch(const msgpack::type_error& e) {
    req.error(msgpack::rpc::ARGUMENT_ERROR, std::string(e.what()));
  } catch(const jubatus::exception::jubatus_exception& e) {
    LOG(WARNING) << e.diagnostic_information(true);
    req.error(std::string(e.what()));
  } catch(const std::exception& e) {
    LOG(ERROR) << e.what();
    req.error(std::string(e.what()));
  }
}

void rpc_server::add_inner(const std::string& name,
    pfi::lang::shared_ptr<invoker_base> invoker) {
  funcs_[name] = invoker;
}

void rpc_server::listen(uint16_t port) {
  instance_.listen(std::string("0.0.0.0"), port);
}

void rpc_server::listen(uint16_t port, const std::string& bind_address) {
  instance_.listen(bind_address, port);
}

void rpc_server::start(int nthreads, bool no_hang) {
  if (no_hang) {
    instance_.start(nthreads);
  } else {
    instance_.run(nthreads);
  }
}

void rpc_server::join() {
  instance_.join();
}

void rpc_server::stop() {
  if (!instance_.is_end()) {
    instance_.end();
    instance_.join();
  }
}

void rpc_server::close() {
  stop();
  instance_.close();
}

}  // namespace mprpc
}  // namespace common
}  // namespace server
}  // namespace jubatus
