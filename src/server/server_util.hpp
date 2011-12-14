// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#pragma once

#include <csignal>
#include <string>
#include <sstream>

#include <msgpack.hpp>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/noncopyable.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>
#include <pficommon/lang/function.h>
#include <pficommon/network/mprpc.h>

#include "../common/util.hpp"

#ifdef HAVE_ZOOKEEPER_H
#  include "mixer.hpp"
#endif

static const std::string VERSION(JUBATUS_VERSION);

#define SET_PROGNAME(s) \
  static const std::string PROGNAME(JUBATUS_APPNAME "_" s);

namespace cmdline{
class parser;
}

namespace jubatus {

struct server_argv {

  server_argv(int args, char** argv);
  server_argv();
  
  bool join;
  int port;
  int timeout;
  int threadnum;
  std::string z;
  std::string name;
  std::string tmpdir;
  std::string eth;
  int interval_sec;
  int interval_count;

  bool is_standalone() const {
    return (z == "");
  };

  std::string boot_message(const std::string& progname) const {
    std::stringstream ret;
    ret << "starting " << progname << VERSION << " RPC server at " <<
      eth << ":" << port << " with timeout: " << timeout;
    return ret.str();
  };
};


struct keeper_argv {
  keeper_argv(int args, char** argv);
  keeper_argv();
  
  int port;
  int timeout;
  int threadnum;
  std::string z;
  std::string eth;

  std::string boot_message(const std::string& progname) const {
    std::stringstream ret;
    ret << "starting " << progname << VERSION << " RPC server at " <<
      eth << ":" << port << " with timeout: " << timeout;
    return ret.str();
  };
};

template <typename From, typename To>
void convert(const From& from, To& to){
  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, from);
  msgpack::unpacked msg;
  msgpack::unpack(&msg, sbuf.data(), sbuf.size());
  msg.get().convert(&to);
}

#ifdef HAVE_ZOOKEEPER_H
extern pfi::lang::shared_ptr<jubatus::common::lock_service> ls;
void atexit(void);

void exit_on_term(int);
void exit_on_term2(int, siginfo_t*, void*);
void set_exit_on_term();
#endif

template <class ImplServerClass, class UserServClass>
int run_server(int args, char** argv){

  ImplServerClass impl_server(server_argv(args, argv));
#ifdef HAVE_ZOOKEEPER_H
  pfi::network::mprpc::rpc_server& serv = impl_server;
  serv.add<std::string(int)>
    ("get_diff",
     pfi::lang::bind(&UserServClass::get_diff_impl,
		     impl_server.get_p().get(), pfi::lang::_1));
  serv.add<int(std::string)>
    ("put_diff",
     pfi::lang::bind(&UserServClass::put_diff_impl,
		     impl_server.get_p().get(), pfi::lang::_1));
  serv.add<std::string(int)>
    ("get_storage",
     pfi::lang::bind(&UserServClass::get_storage,
		     impl_server.get_p().get(), pfi::lang::_1));

  set_exit_on_term();

#endif // HAVE_ZOOKEEPER_H
  return impl_server.run();
};

}; // end jubatus
