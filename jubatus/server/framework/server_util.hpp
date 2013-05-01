// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <glog/logging.h>
#include <msgpack.hpp>
#include <pficommon/lang/noncopyable.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>
#include <pficommon/lang/function.h>
#include <pficommon/lang/shared_ptr.h>

#include "jubatus/core/common/exception.hpp"
#include "../common/util.hpp"
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

  bool join;
  int port;
  std::string bind_address;
  std::string bind_if;
  int timeout;
  int threadnum;
  std::string program_name;
  std::string type;
  std::string z;
  std::string name;
  std::string datadir;
  std::string logdir;
  int loglevel;
  std::string configpath;
  std::string eth;
  int interval_sec;
  int interval_count;

  MSGPACK_DEFINE(join, port, bind_address, bind_if, timeout, threadnum,
      program_name, type, z, name, datadir, logdir, loglevel, eth,
      interval_sec, interval_count);

  bool is_standalone() const {
    return (z == "");
  }
  void boot_message(const std::string& progname) const;
  void set_log_destination(const std::string& progname) const;
};

std::string get_server_identifier(const server_argv& a);

struct keeper_argv {
  keeper_argv(int args, char** argv, const std::string& t);
  keeper_argv();

  int port;
  std::string bind_address;
  std::string bind_if;
  int timeout;
  int threadnum;
  std::string program_name;
  std::string z;
  std::string logdir;
  int loglevel;
  std::string eth;
  const std::string type;
  int session_pool_expire;
  int session_pool_size;

  void boot_message(const std::string& progname) const;
  void set_log_destination(const std::string& progname) const;
};

template<typename From, typename To>
void convert(const From& from, To& to) {
  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, from);
  msgpack::unpacked msg;
  msgpack::unpack(&msg, sbuf.data(), sbuf.size());
  msg.get().convert(&to);
}

extern pfi::lang::shared_ptr<
    jubatus::server::common::lock_service> ls;
void atexit();

template<class ImplServerClass>
int run_server(int args, char** argv, const std::string& type) {
  try {
    ImplServerClass impl_server(server_argv(args, argv, type));

    impl_server.get_p()->get_mixer()->register_api(impl_server);
    ::atexit(jubatus::server::framework::atexit);

    jubatus::util::set_exit_on_term();
    jubatus::util::ignore_sigpipe();
    return impl_server.run();
  } catch (const jubatus::exception::jubatus_exception& e) {
    LOG(FATAL) << e.diagnostic_information(true);
    return -1;
  }
}

std::string get_conf(const server_argv& a);

}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_SERVER_UTIL_HPP_
