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
#include <cstring>
#include <map> //for pair

#include <pficommon/network/mprpc.h>
#include <pficommon/lang/function.h>

#include "../common/zk.hpp"
#include "../common/membership.hpp"
#include "../common/cmdline.h"

static const std::string PROGNAME(JUBATUS_APPNAME "ctl");
static const std::string VERSION(JUBATUS_VERSION);

using std::string;
using std::cout;
using std::endl;
using std::vector;
void usage();
void send2supervisor(const string& , const string&, const string&, unsigned int N);
void send2server(const string& , const string&, const string&);
void status(const string&, const string&);

int main(int args, char** argv){
  cmdline::parser p;
  p.add<std::string>("cmd", 'c', "command to send servers(start|stop|save|load)", true);
  p.add<std::string>("server", 's', "server exec file of learning machine (jubaclassifier, ...)", true);
  p.add<std::string>("name", 'n', "learning machine name", true);
  p.add<unsigned int>("num", 'N', "num of process in the whole cluster (one on each server when 0)", false);
  p.add<std::string>("zookeeper", 'z', "ZooKeeper location environment: 'ZK' is available instead", false);
  p.add("debug", 'd', "debug mode");
  p.parse_check(args, argv);
  google::InitGoogleLogging(argv[0]);
  if(p.exist("debug")){
    google::LogToStderr(); // only when debug
  }

  string cmd = p.get<std::string>("cmd");
  string name = p.get<std::string>("server") + "/" + p.get<std::string>("name");

  unsigned int N = p.get<unsigned int>("num");


  string zk;
  char* zkenv = std::getenv("ZK");

  if(zkenv){
    zk = zkenv;
  }else if(p.get<std::string>("zookeeper") != "" ){
    zk = p.get<std::string>("zookeeper");
  }else{
    cout << "can't get ZK location: set 'ZK' environment or specify '-z <somezkaddrs>" << endl;
    cout << p.get<std::string>("zookeeper") << endl;;
    cout << p.usage() << endl;
    exit(1);
  }

  if(cmd == "status"){
    status(p.get<string>("name"), zk);
    return 0;
  }

  if(cmd == "start" or cmd == "stop"){
    send2supervisor(cmd, name, zk, N);
  }else if(cmd == "save" or cmd == "load"){ //or set_config?
    send2server(cmd, name, zk);
  }
  //cout << cmd << name << zk << endl;
}

int do_request(const string& cmd, const string& name, const string& ip_port, unsigned int n){
  string ip;
  int port;
  jubatus::common::revert(ip_port, ip, port);
  
  pfi::network::mprpc::rpc_client c(ip, port, 10);
  pfi::lang::function<int(string,unsigned int)> f = c.call<int(string,unsigned int)>(cmd);
  cout << "sending " << cmd << " / " << name << " to " << ip_port << "..." << std::flush;
  int r = f(name, n);
  
  if(r!=0){
    cout << "faled." << endl;
    LOG(ERROR) << "can't do '" << cmd << " " << name << "' in " << ip << " " << port << ":" << r;
  }else{
    cout << "ok." << endl;
  }
  return r;
}

void send2supervisor(const string& cmd, const string& name, const string& zkhosts, unsigned int N){
  pfi::lang::shared_ptr<jubatus::common::lock_service> ls_
    (jubatus::common::create_lock_service("zk", zkhosts, 10, "/dev/null"));

  vector<string> list;

  if(cmd == "start"){ //FIXME: mess
    string path = name.substr(name.find_first_of("/")+1);
    path = jubatus::common::ACTOR_BASE_PATH + "/" + path.substr(0, path.find_first_of("/"));
    ls_->create(path);
    path += "/nodes";
    ls_->create(path);
  }

  ls_->list(jubatus::common::JUBAVISOR_BASE_PATH, list);

  if(list.empty()){
    LOG(INFO) << "no server to " << cmd << " " << name;
    exit(-1);
  }
  if(N == 0){
    N = list.size();
  }
  vector<string>::const_iterator it;
  unsigned int n = N / list.size();
  for(it = list.begin(); it!= list.end(); ++it){
    do_request(cmd, name, *it, n);
  }
  n = N % list.size();
  for(size_t i = 0; i < n; ++i){
    do_request(cmd, name, list[i], 1);
  }
}

void send2server(const string& cmd, const string& name, const string& zkhosts){
  pfi::lang::shared_ptr<jubatus::common::lock_service> ls_
    (jubatus::common::create_lock_service("zk", zkhosts, 10, "/dev/null"));
  vector<string> list;
  ls_->list(jubatus::common::ACTOR_BASE_PATH, list);
  
  if(list.empty()){
    LOG(INFO) << "no server to " << cmd << " " << name;
  }
  vector<string>::const_iterator it;
  for(it = list.begin(); it!= list.end(); ++it){
    string ip;
    int port;
    jubatus::common::revert(*it, ip, port);
    
    pfi::network::mprpc::rpc_client c(ip, port, 10);
    pfi::lang::function<int(string)> f = c.call<int(string)>(cmd);
    cout << "sending " << cmd << " / " << name << " to " << *it << "..." << std::flush;
    int r = f(name);
    if(r!=0){
      cout << "failed." << endl;
      LOG(ERROR) << "can't do '" << cmd << " " << name << "' in " << ip << " " << port;
    }else{
      cout << "ok." << endl;
    }
  }
}


void show(jubatus::common::lock_service& z, const string& path, const string& name){
  vector<string> list;
  z.list(path, list);
  cout << "\033[34mactive " << name << " members:\033[0m" << endl;
  for(size_t i=0; i<list.size(); ++i){
    cout << list[i] << endl;
  }
}

void status(const string& name, const string& zkhosts){
  pfi::lang::shared_ptr<jubatus::common::lock_service> ls_
    (jubatus::common::create_lock_service("zk", zkhosts, 10, "/dev/null"));
  show(*ls_, jubatus::common::JUBAKEEPER_BASE_PATH, "jubakeeper");
  show(*ls_, jubatus::common::JUBAVISOR_BASE_PATH, "jubavisor");
  show(*ls_, jubatus::common::ACTOR_BASE_PATH + "/" + name + "/nodes", name);
}
