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

#include "jubakeeper.hpp"
#include "rpc.hpp"

#include <glog/logging.h>
#include <iostream>
#include <pficommon/lang/bind.h>
#include "../common/cmdline.h"
#include "../common/util.hpp"

using namespace jubatus;
using namespace pfi::lang;

static const std::string PROGNAME = "jubakeeper";

int main(int argc, char* argv[]){
  
  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9198);
  p.add<int>("thread", 't', "thread number", false, 2);
  p.add<std::string>("zookeeper", 'z', "zookeeper location", true);
  p.add("daemon", 'd', "daemonize the process");

  p.parse_check(argc,argv);
  int port = p.get<int>("rpc-port");
  std::string logfile = "";

  google::InitGoogleLogging(argv[0]);
  if(p.exist("daemon")){
    util::daemonize();
    logfile = std::string("/tmp/") + PROGNAME + ".zklog";
    LOG(INFO) << "zk logging to " << logfile;
  }else{
    google::LogToStderr(); // only when debug
  }
  
  jubakeeper j(p.get<std::string>("zookeeper"), port, 10, logfile);
  jubakeeper_server serv(10*1024);
  {
    serv.set_lookup(bind(&jubakeeper::lookup, &j, _1));
    serv.set_lookup_cht(bind(&jubakeeper::lookup_cht, &j, _1, _2));

    serv.set_set_config(bind(&jubakeeper::set_config, &j, _1, _2));
    serv.set_get_config(bind(&jubakeeper::get_config, &j, _1));
    serv.set_train(bind(&jubakeeper::train, &j, _1, _2));
    serv.set_classify(bind(&jubakeeper::classify, &j, _1, _2));
    serv.set_save(bind(&jubakeeper::save, &j, _1, _2, _3));
    serv.set_load(bind(&jubakeeper::save, &j, _1, _2, _3));
    serv.set_get_status(bind(&jubakeeper::get_status, &j, _1));

    if(!serv.serv(port, p.get<int>("thread"))){
      LOG(ERROR) << "failed starting server";
      return -1;
    }
  }
  return 0;
}

