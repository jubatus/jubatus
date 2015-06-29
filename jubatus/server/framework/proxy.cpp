// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include "proxy.hpp"

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "jubatus/core/common/assert.hpp"
#include "jubatus/core/common/exception.hpp"
#include "aggregators.hpp"
#include "../common/logger/logger.hpp"
#include "../common/membership.hpp"
#include "../common/signals.hpp"
#include "../common/system.hpp"
#include "server_util.hpp"

namespace jubatus {
namespace server {
namespace framework {

__thread proxy::async_task_loop*
  proxy::async_task_loop::private_async_task_loop_ = NULL;

// NOTE: '__thread' is gcc-extension. We should re-implement with
//       pthread TLS?

proxy::proxy(const proxy_argv& a)
    : proxy_common(a),
      jubatus::server::common::mprpc::rpc_server(a.timeout) {
  // register default methods
  register_async_random<std::string>("get_config");
  register_async_broadcast<std::map<std::string, std::string>, std::string>(
      "save",
      jubatus::util::lang::function<
          std::map<std::string, std::string>(
            std::map<std::string, std::string>,
            std::map<std::string, std::string>)>(
          &jubatus::server::framework::merge<std::string, std::string>));
  register_async_broadcast<bool, std::string>(
      "load",
      jubatus::util::lang::function<bool(bool, bool)>(
          &jubatus::server::framework::all_and));
  register_async_broadcast<status_type>(
      "get_status",
      jubatus::util::lang::function<status_type(status_type, status_type)>(
          &jubatus::server::framework::merge<std::string, string_map>));
  rpc_server::add<status_type()>(
      "get_proxy_status",
      jubatus::util::lang::bind(&proxy::get_status, this));
}

proxy::~proxy() {
}

namespace {

void stop_rpc_server(msgpack::rpc::server& serv) {
  LOG(INFO) << "stopping RPC server";
  serv.end();
}

}  // anonymous namespace

int proxy::run() {
  try {
    this->instance_.listen(a_.bind_address, a_.port);
    LOG(INFO) << "start listening at port " << a_.port;
    start_time_= jubatus::util::system::time::get_clock_time();
    this->instance_.start(a_.threadnum);

    // RPC server started, then register group membership
    register_proxy(*zk_, a_.type, a_.eth, a_.port);
    LOG(INFO) << "registered group membership";

    LOG(INFO) << common::get_program_name() << " RPC server startup";

    common::set_action_on_term(
        jubatus::util::lang::bind(&stop_rpc_server,
            jubatus::util::lang::ref(this->instance_)));

    this->instance_.join();

    return 0;
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    LOG(FATAL) << "exception when starting RPC server: "
               << e.diagnostic_information(true);
  } catch (const mp::system_error& e) {
    if (e.code == EADDRINUSE) {
      LOG(FATAL) << "server failed to start: " << e.what()
                 << " (any process using port " << a_.port << "?)";
    } else {
      LOG(FATAL) << "server failed to start: " << e.what();
    }
  } catch (const std::exception& e) {
    LOG(FATAL) << "error when starting RPC server: " << e.what();
  }
  return -1;
}

std::string proxy::get_error_message(
    const jubatus::server::common::mprpc::rpc_error& err) {
  try {
    err.throw_exception();
    JUBATUS_ASSERT_UNREACHABLE();
  } catch (const jubatus::server::common::mprpc::rpc_call_error& e) {
    jubatus::core::common::exception::error_info_list_t info =
        e.error_info();
    for (size_t i = 0; i < info.size(); ++i) {
      using jubatus::core::common::exception::error_message;
      error_message* m = dynamic_cast<error_message*>(info[i].get());
      if (m) {
        return m->value();
      }
    }
    return e.what();
  } catch (const std::exception& e) {
    return e.what();
  } catch (...) {
    return "unknown error";
  }
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
