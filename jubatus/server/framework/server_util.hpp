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

#ifndef JUBATUS_SERVER_FRAMEWORK_SERVER_UTIL_HPP_
#define JUBATUS_SERVER_FRAMEWORK_SERVER_UTIL_HPP_

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include <msgpack.hpp>
#include "jubatus/util/lang/noncopyable.h"
#include "jubatus/util/concurrent/lock.h"
#include "jubatus/util/concurrent/rwmutex.h"
#include "jubatus/util/lang/function.h"
#include "jubatus/util/lang/shared_ptr.h"

#include "jubatus/core/common/exception.hpp"
#include "../common/logger/logger.hpp"
#include "../common/system.hpp"
#include "../common/lock_service.hpp"

namespace cmdline {
class parser;
}

namespace jubatus {

namespace core {
namespace fv_converter {
class datum_to_fv_converter;
}  // namespace fv_converter
}  // namespace core

namespace server {
namespace framework {

template <class S>
class server_helper;

struct config_json {
  config_json() {
  }

  std::string config;

  void load_json(
      const std::string& zkhosts,
      const std::string& type,
      const std::string& name);
  void load_json(const std::string& filepath);
};

struct server_argv {
  server_argv(int args, char** argv, const std::string& type);
  server_argv();

  int port;
  std::string bind_address;
  std::string bind_if;
  int timeout;
  int zookeeper_timeout;
  int interconnect_timeout;
  int threadnum;
  std::string program_name;
  std::string type;
  std::string z;
  std::string name;
  std::string datadir;
  std::string logdir;
  std::string log_config;
  std::string configpath;
  std::string modelpath;
  std::string eth;
  int interval_sec;
  int interval_count;
  std::string mixer;
  bool daemon;
  bool config_test;

  MSGPACK_DEFINE(port, bind_address, bind_if, timeout,
      zookeeper_timeout, interconnect_timeout, threadnum,
      program_name, type, z, name, datadir, logdir, log_config, eth,
      interval_sec, interval_count, mixer, daemon, config_test);

  bool is_standalone() const {
    return (z == "");
  }
  void boot_message(const std::string& progname) const;
};

std::string get_server_identifier(const server_argv& a);

struct proxy_argv {
  proxy_argv(int args, char** argv, const std::string& t);
  proxy_argv();

  int port;
  std::string bind_address;
  std::string bind_if;
  int timeout;
  int zookeeper_timeout;
  int interconnect_timeout;
  int threadnum;
  std::string program_name;
  std::string z;
  std::string logdir;
  std::string log_config;
  std::string eth;
  const std::string type;
  int session_pool_expire;
  int session_pool_size;
  bool daemon;

  void boot_message(const std::string& progname) const;
};

std::string get_proxy_identifier(const proxy_argv& a);

void register_lock_service(
    jubatus::util::lang::shared_ptr<common::lock_service> ls);
void close_lock_service();

template<class ImplServerClass, class ServerClass>
int run_server(int args, char** argv, const std::string& type) {
  try {
    server_argv parsed_argv(args, argv, type);
    if (parsed_argv.config_test) {
      try {
        jubatus::server::framework::server_helper<ServerClass> sh(parsed_argv);
      } catch (const jubatus::core::common::exception::jubatus_exception& e) {
        std::cerr << "Configuration error:" << std::endl
                  << e.diagnostic_information(true);
        return 1;
      }
      std::cerr << "Configuration OK" << std::endl;
      return 0;
    }

    ImplServerClass impl_server(parsed_argv);
    if (!parsed_argv.is_standalone()) {
      impl_server.get_p()->get_mixer()->register_api(impl_server);
    }
    return impl_server.run();
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    if (common::logger::is_configured()) {
      LOG(FATAL) << "exception in main thread: "
                 << e.diagnostic_information(true);
    } else {
      std::cerr << "exception in main thread: "
                << e.diagnostic_information(true);
    }
    return -1;
  } catch (const std::exception& e) {
    if (common::logger::is_configured()) {
      LOG(FATAL) << "error in main thread: " << e.what();
    } else {
      std::cerr << "error in main thread: " << e.what();
    }
    return -1;
  }
}

std::string get_conf(const server_argv& a);

}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_SERVER_UTIL_HPP_
