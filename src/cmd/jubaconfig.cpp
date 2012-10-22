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

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>

#include <pficommon/network/mprpc.h>
#include <pficommon/lang/function.h>

#include "../common/exception.hpp"
#include "../common/zk.hpp"
#include "../common/membership.hpp"
#include "../common/config.hpp"
#include "../common/cmdline.h"
#include "../framework/server_util.hpp"

static const std::string PROGNAME(JUBATUS_APPNAME "config");
static const std::string VERSION(JUBATUS_VERSION);

using std::string;
using std::cout;
using std::endl;
using std::vector;
void usage();
void set_config(const string&, const string&, const string&, const string&);
void get_config(const string&, const string&, const string&);

int main(int args, char** argv) try {
  cmdline::parser p;
  p.add<std::string>("file", 'f', "config file to set", false, "");
  p.add<std::string>("type", 't', "learning machine type", true);
  p.add<std::string>("name", 'n', "learning machine name", true);
  p.add<std::string>("zookeeper", 'z', "ZooKeeper location environment: 'ZK' is available instead", false);

  p.add("debug", 'd', "debug mode");
  p.parse_check(args, argv);
  google::InitGoogleLogging(argv[0]);
  if(p.exist("debug")){
    google::LogToStderr(); // only when debug
  }

  string zk;
  char* zkenv = std::getenv("ZK");

  if(p.get<std::string>("zookeeper") != ""){
    zk = p.get<std::string>("zookeeper");
  }else if(zkenv){
    zk = zkenv;
  }else{
    cout << "can't get ZK location: set 'ZK' environment or specify '-z <somezkaddrs>" << endl;
    cout << p.get<std::string>("zookeeper") << endl;;
    cout << p.usage() << endl;
    exit(1);
  }

  

  string cf = p.get<std::string>("file");

  if(cf == ""){
    get_config(zk , p.get<std::string>("type"), p.get<std::string>("name"));
  }
  else{
    set_config(zk , p.get<std::string>("type"), p.get<std::string>("name"), cf);
  }

  return 0;
} catch (const jubatus::exception::jubatus_exception& e) {
  std::cout << e.diagnostic_information(true) << std::endl;
}

void set_config(const string& zkhosts, 
                const string& type, const string& name, const string& configfile){
  pfi::lang::shared_ptr<jubatus::common::lock_service> ls_
    (jubatus::common::create_lock_service("zk", zkhosts, 10, "test.log"));

  jubatus::common::prepare_jubatus(*ls_, type, "");
  std::ifstream ifc(configfile.c_str());
  std::stringstream ss;
  ss << ifc.rdbuf();
  string config_str = ss.str();

  if (!ifc){
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("can't read config file."));
  }
  jubatus::common::setconfig_tozk(*ls_, type, name, config_str);

}

void get_config(const string& zkhosts,
                const string& type, const string& name){
  pfi::lang::shared_ptr<jubatus::common::lock_service> ls_
    (jubatus::common::create_lock_service("zk", zkhosts, 10, "/dev/null"));

  string config_str;
  jubatus::common::getconfig_fromzk(*ls_, type, name, config_str);
  std::cout << config_str << std::endl;
}


