// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

#include <msgpack.hpp>

#include "../common/exception.hpp"
#include "../common/shared_ptr.hpp"
#include "../common/util.hpp"
#include <pficommon/lang/noncopyable.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>
#include <pficommon/lang/function.h>
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/shared_ptr.h>

#ifdef HAVE_ZOOKEEPER_H
#  include "../common/lock_service.hpp"
#endif

namespace cmdline{
class parser;
}

namespace jubatus {

namespace fv_converter {
class datum_to_fv_converter;
}

namespace framework {

struct server_argv {

  server_argv(int args, char** argv);
  server_argv();
  
  bool join;
  int port;
  int timeout;
  int threadnum;
  std::string program_name;
  std::string z;
  std::string name;
  std::string tmpdir;
  std::string eth;
  int interval_sec;
  int interval_count;

  bool is_standalone() const {
    return (z == "");
  }
  std::string boot_message(const std::string& progname) const;
};


struct keeper_argv {
  keeper_argv(int args, char** argv);
  keeper_argv();
  
  int port;
  int timeout;
  int threadnum;
  std::string z;
  std::string eth;

  std::string boot_message(const std::string& progname) const;
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
extern jubatus::common::cshared_ptr<jubatus::common::lock_service> ls;
void atexit(void);
#endif

template <class ImplServerClass, class UserServClass>
int run_server(int args, char** argv)
{
  try {
    ImplServerClass impl_server(server_argv(args, argv));
#ifdef HAVE_ZOOKEEPER_H
    pfi::network::mprpc::rpc_server& serv = impl_server;
    serv.add<std::vector<std::string>(int)>
      ("get_diff",
       pfi::lang::bind(&UserServClass::get_diff_impl,
           impl_server.get_p().get(), pfi::lang::_1));
    serv.add<int(std::vector<std::string>)>
      ("put_diff",
       pfi::lang::bind(&UserServClass::put_diff_impl,
           impl_server.get_p().get(), pfi::lang::_1));
    serv.add<std::string()>
      ("get_storage",
       pfi::lang::bind(&UserServClass::get_storage,
           impl_server.get_p().get()));

    jubatus::util::set_exit_on_term();
    ::atexit(jubatus::framework::atexit);

#endif // HAVE_ZOOKEEPER_H
    jubatus::util::ignore_sigpipe();
    return impl_server.run();
  } catch (const jubatus::exception::jubatus_exception& e) {
    std::cout << e.diagnostic_information(true) << std::endl;
    return -1;
  }
}

pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter>
make_fv_converter(const std::string& config);

}}
