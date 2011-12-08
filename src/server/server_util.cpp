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

#include "server_util.hpp"
#include "../common/cmdline.h"
#include "../common/exception.hpp"
#include <glog/logging.h>

namespace jubatus {
  
  server_argv::server_argv(int args, char** argv){
    google::InitGoogleLogging(argv[0]);
    // google::LogToStderr(); // only when debug

    cmdline::parser p;
    p.add<int>("rpc-port", 'p', "port number", false, 9199);
    p.add<int>("thread", 'c', "concurrency = thread number", false, 2);
    p.add<int>("timeout", 't', "time out (sec)", false, 10);

    p.add<std::string>("zookeeper", 'z', "zookeeper location", false);
    p.add<std::string>("name", 'n', "learning machine instance name", false);
    p.add<std::string>("tmpdir", 'd', "directory to place plugins", false, "/tmp");
    p.add("join", 'j', "join to the existing cluster");

    p.add<int>("interval_sec", 's', "mix interval by seconds", false, 5);
    p.add<int>("interval_count", 'i', "mix interval by update count", false, 1024);

    p.parse_check(args, argv);

    port = p.get<int>("rpc-port");
    threadnum = p.get<int>("thread");
    timeout = p.get<int>("timeout");
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
    
    LOG(INFO) << boot_message(argv[0]);
  };

  server_argv::server_argv():
    join(false), port(9199), timeout(10), threadnum(2), z(""), name(""),
    tmpdir("/tmp"), eth("localhost"), interval_sec(5), interval_count(1024)
  {
  };

  keeper_argv::keeper_argv(int args, char** argv){
    google::InitGoogleLogging(argv[0]);
    //    google::LogToStderr(); // only when debug

    cmdline::parser p;
    p.add<int>("rpc-port", 'p', "port number", false, 9199);
    p.add<int>("thread", 'c', "concurrency = thread number", false, 16);
    p.add<int>("timeout", 't', "time out (sec)", false, 10);

    p.add<std::string>("zookeeper", 'z', "zookeeper location", false, "localhost:2181");

    p.parse_check(args, argv);

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
}
