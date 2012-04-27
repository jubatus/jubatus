// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "jubavisor.hpp"

#include <iostream>

#include <glog/logging.h>
#include <pficommon/lang/bind.h>
#include <assert.h>

#include "../common/util.hpp"
#include "../common/cmdline.h"

static const std::string PROGNAME = "jubakeeper";

using namespace jubatus;
using namespace pfi::lang;

int main(int argc, char* argv[]){
  
  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9199);
  p.add<std::string>("zookeeper", 'z', "zookeeper location", true);
  p.add<std::string>("logdir", 'l', "log to output all child process' log", false, "/tmp");
  p.add<int>("timeout", 't', "rpc timeout", false, 10);
  p.add("daemon", 'd', "daemonize the process");
  p.parse_check(argc,argv);
  int port = p.get<int>("rpc-port");
  std::string logfile = "";
  
  if(p.exist("daemon")){
    util::append_server_path(argv[0]);
    logfile = p.get<std::string>("logdir") + "/" + PROGNAME + ".";

    int r = util::daemonize();

    google::InitGoogleLogging(argv[0]);
    if(r == -1){
      LOG(ERROR) << "failed at daemon(3): errno=" << errno; 
      return -1;
    }
    logfile += "zklog";
    LOG(INFO) << " starting at daemon mode: port=" << port;
    LOG(INFO) << " zk logging to " << logfile;
  }
  else{
    google::InitGoogleLogging(argv[0]);
    google::LogToStderr(); // only when debug
    LOG(INFO) << " starting at standalone mode " << port;
  }
  
  jubervisor j(p.get<std::string>("zookeeper"), port, 16, logfile);
  jubervisor_server serv(10*1024);
  {
    serv.set_start(bind(&jubervisor::start, &j, _1, _2));
    serv.set_stop(bind(&jubervisor::stop, &j, _1, _2));
    if(!serv.serv(port, 2)){
      LOG(ERROR) << "cannot start rpc server.";
      return -1;
    }
  }
  return 0;
}

