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

#include "server_util.hpp"
#include <glog/logging.h>

#include <iostream>

#include "../common/util.hpp"
#include "../common/cmdline.h"
#include "../common/exception.hpp"
#include "../common/membership.hpp"


#define SET_PROGNAME(s) \
  static const std::string PROGNAME(JUBATUS_APPNAME "_" s);


namespace jubatus { namespace framework {

  static const std::string VERSION(JUBATUS_VERSION);

  void print_version(const std::string& progname){
    std::cout << "jubatus-" << VERSION << " (" << progname << ")" << std::endl;
  }

  server_argv::server_argv(int args, char** argv){
    google::InitGoogleLogging(argv[0]);
    google::LogToStderr(); // only when debug

    cmdline::parser p;
    p.add<int>("rpc-port", 'p', "port number", false, 9199);
    p.add<int>("thread", 'c', "concurrency = thread number", false, 2);
    p.add<int>("timeout", 't', "time out (sec)", false, 10);

    p.add<std::string>("zookeeper", 'z', "zookeeper location", false);
    p.add<std::string>("name", 'n', "learning machine instance name", false);
    p.add<std::string>("tmpdir", 'd', "directory to place plugins", false, "/tmp");
    p.add("join", 'j', "join to the existing cluster");

    p.add<int>("interval_sec", 's', "mix interval by seconds", false, 16);
    p.add<int>("interval_count", 'i', "mix interval by update count", false, 512);

    p.add("version", 'v', "version");

    p.parse_check(args, argv);

    std::cout<< "program name = " << jubatus::util::get_program_name() << std::endl;
    if( p.exist("version") ){
      print_version(jubatus::util::get_program_name());
      exit(0);
    }

    port = p.get<int>("rpc-port");
    threadnum = p.get<int>("thread");
    timeout = p.get<int>("timeout");
    program_name = jubatus::util::get_program_name();
    z = p.get<std::string>("zookeeper");
    name = p.get<std::string>("name");
    tmpdir = p.get<std::string>("tmpdir");
    //    eth = "localhost";
    eth = jubatus::util::get_ip("eth0");
    join = p.exist("join");

    interval_sec = p.get<int>("interval_sec");
    interval_count = p.get<int>("interval_count");

    if(z != "" and name == ""){
      throw argv_error("can't start multinode mode without name specified");
    }
    
    LOG(INFO) << boot_message(jubatus::util::get_program_name());
  };

  server_argv::server_argv():
    join(false), port(9199), timeout(10), threadnum(2), z(""), name(""),
    tmpdir("/tmp"), eth("localhost"), interval_sec(5), interval_count(1024)
  {
  };

  std::string server_argv::boot_message(const std::string& progname) const {
    std::stringstream ret;
    ret << "starting " << progname << VERSION << " RPC server at " <<
      eth << ":" << port << " with timeout: " << timeout;
    return ret.str();
  };

  keeper_argv::keeper_argv(int args, char** argv){
    google::InitGoogleLogging(argv[0]);
    google::LogToStderr(); // only when debug

    cmdline::parser p;
    p.add<int>("rpc-port", 'p', "port number", false, 9199);
    p.add<int>("thread", 'c', "concurrency = thread number", false, 16);
    p.add<int>("timeout", 't', "time out (sec)", false, 10);

    p.add<std::string>("zookeeper", 'z', "zookeeper location", false, "localhost:2181");
    p.add("version", 'v', "version");

    p.parse_check(args, argv);

    if( p.exist("version") ){
      print_version(jubatus::util::get_program_name());
      exit(0);
    }

    port = p.get<int>("rpc-port");
    threadnum = p.get<int>("thread");
    timeout = p.get<int>("timeout");
    z = p.get<std::string>("zookeeper");
    eth = jubatus::util::get_ip("eth0");
  };

  keeper_argv::keeper_argv():
    port(9199), timeout(10), threadnum(16), z("localhost:2181"), eth("")
  {
  };

  std::string keeper_argv::boot_message(const std::string& progname) const {
    std::stringstream ret;
    ret << "starting " << progname << VERSION << " RPC server at " <<
      eth << ":" << port << " with timeout: " << timeout;
    return ret.str();
  };

  
#ifdef HAVE_ZOOKEEPER_H
    common::cshared_ptr<jubatus::common::lock_service> ls;

  void atexit(void){
    if(ls)
      ls->force_close();
  }

  void exit_on_term(int){
    exit(0);
  }
  void exit_on_term2(int, siginfo_t*, void*){
    exit(0);
  }

  void set_exit_on_term(){
    struct sigaction sigact;
    sigact.sa_handler = exit_on_term;
    sigact.sa_sigaction = exit_on_term2;
    sigact.sa_flags = SA_RESTART;
    if(sigaction(SIGTERM, &sigact, NULL) != 0)
      throw std::runtime_error("can't set SIGTERM handler");
    if(sigaction(SIGINT, &sigact, NULL) != 0)
      throw std::runtime_error("can't set SIGINT handler");

    ::atexit(jubatus::framework::atexit);
  }
#endif

}
}
