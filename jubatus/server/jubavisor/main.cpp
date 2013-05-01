// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <iostream>
#include <string>

#include <glog/logging.h>
#include <pficommon/lang/bind.h>

#include "jubatus/core/common/exception.hpp"
#include "../third_party/cmdline/cmdline.h"
#include "../common/util.hpp"

#include "jubavisor.hpp"

namespace {
const std::string PROGNAME = "jubavisor";
}  // namespace

using jubatus::server::jubavisor::jubavisor;
using jubatus::server::jubavisor::jubavisor_server;
using pfi::lang::_1;
using pfi::lang::_2;
using pfi::lang::_3;
using pfi::lang::bind;

int main(int argc, char* argv[]) try {
  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9198);
  p.add<std::string>("zookeeper", 'z', "zookeeper location", true);
  p.add<std::string>("logdir", 'l', "log to output all child process' log",
                     false, "/tmp");
  p.add<int>("timeout", 't', "rpc timeout", false, 10);
  p.add("daemon", 'd', "daemonize the process");
  p.parse_check(argc, argv);
  int port = p.get<int>("rpc-port");
  std::string logfile = "";

  if (p.exist("daemon")) {
    jubatus::util::append_server_path(argv[0]);
    logfile = p.get<std::string>("logdir") + "/" + PROGNAME + ".";

    int r = jubatus::util::daemonize();

    google::InitGoogleLogging(argv[0]);
    if (r == -1) {
      LOG(ERROR) << "failed at daemon(3): errno=" << errno;
      return -1;
    }
    logfile += "zklog";
    LOG(INFO) << " starting at daemon mode: port=" << port;
    LOG(INFO) << " zk logging to " << logfile;
  } else {
    google::InitGoogleLogging(argv[0]);
    google::LogToStderr();  // only when debug
    LOG(INFO) << " starting at standalone mode " << port;
  }

  jubavisor j(p.get<std::string>("zookeeper"), port, 16, logfile);
  jubavisor_server serv(10 * 1024);
  {
    serv.set_start(bind(&jubavisor::start, &j, _1, _2, _3));
    serv.set_stop(bind(&jubavisor::stop, &j, _1, _2));

    try {
      serv.rpc_listen(port);
      serv.rpc_start(2);
      serv.rpc_join();
    } catch (const mp::system_error& e) {
      if (e.code == EADDRINUSE) {
        LOG(ERROR) << "server failed to start: any process using port "
                   << port << "?";
      } else {
        LOG(FATAL) << "server failed to start: " << e.what();
      }
      return -1;
    }
  }
  return 0;
} catch (const jubatus::exception::jubatus_exception& e) {
  std::cout << e.diagnostic_information(true) << std::endl;
}
