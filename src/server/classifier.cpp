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

#include <string>

#include <pficommon/lang/bind.h>
#include <glog/logging.h>

#include "../common/cmdline.h"
#include "../common/util.hpp"

#ifdef HAVE_ZOOKEEPER_H
#  include "../common/zk.hpp"
#  include "../common/membership.hpp"
#endif
#include "./storage/storage_factory.hpp"
#include "classifier_serv.hpp"
#include "classifier_rpc.hpp"

using namespace std;
using namespace pfi::lang;
using namespace jubatus;

static const string PROGNAME(JUBATUS_APPNAME "_classifier");
static const string VERSION(JUBATUS_VERSION);

int start_all(classifier::server &s, const cmdline::parser& p){

  classifier::mprpc_server serv(p.get<int>("timeout"));
  // run msgpack-rpc server
  {
    s.bind_all_methods(serv, util::get_ip("eth0"), p.get<int>("rpc-port"));
    if(!serv.serv(p.get<int>("rpc-port"), p.get<int>("thread"))){
      LOG(ERROR) << "failed starting server";
      return -1;
    }
  }
  return 0;
}

int main(int argc, char *argv[])
{

  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9199);
  p.add<int>("thread", 'c', "thread number", false, 2);
  p.add<int>("timeout", 't', "time out", false, 10);
  p.add<std::string>("storage", 'S', "storage type", false, "local");
  p.add<std::string>("zookeeper", 'z', "zookeeper location", false);
  p.add<std::string>("name", 'n', "learning machine instance name", true);
  p.add<std::string>("tmpdir", 'd', "directory to place plugins", false, "/tmp");

  p.parse_check(argc, argv);
  int port = p.get<int>("rpc-port");
  int timeout = p.get<int>("timeout");
  std::string logfile = "/tmp/";
  logfile += PROGNAME + ".zklog";

  google::InitGoogleLogging(argv[0]);
  //  google::LogToStderr(); // only when debug

  std::string self(jubatus::util::get_ip("eth0"));
  std::string name(p.get<std::string>("name"));

  LOG(INFO) << "starting " << PROGNAME << " " << VERSION << " with storage: " << p.get<std::string>("storage");
  LOG(INFO) << "mprpc-server at " << self << ":" << port << " timeout:" << timeout;

  shared_ptr<storage::storage_base,
             pfi::concurrent::threading_model::multi_thread> st;
  
#ifdef HAVE_ZOOKEEPER_H

  // running on multiple server mode
  if(p.get<std::string>("storage") == "local_mixture"){
    
    std::string zkcluster = (p.get<std::string>("zookeeper") == "")? "localhost:2181" : p.get<std::string>("zookeeper");
    
    shared_ptr<jubatus::zk,
               pfi::concurrent::threading_model::multi_thread> z(new jubatus::zk(zkcluster, timeout, logfile.c_str()));
    st.reset(storage::storage_factory::create_storage(p.get<std::string>("storage")));

    vector<string> list;
    string path = ACTOR_BASE_PATH + "/" + name + "/nodes";
    z->list(path, list);
    if(not list.empty()){
      zkmutex zlk(z, ACTOR_BASE_PATH + "/" + name + "/master_lock");
      while(not zlk.try_lock()){ ; }
      size_t i = rand() % list.size();
      string ip;
      int port;
      revert(list[i], ip, port);
      classifier::mprpc_client c(ip, port, timeout);
      result<string> s = c.call_get_storage(0);
      if(s.success){
	stringstream ss(s.retval);
	st->load(ss);
      }
    }
    register_actor(*z, name, self, port);

    shared_ptr<mixer,
               pfi::concurrent::threading_model::multi_thread> m(new mixer(z, name, &classifier::server::mix));
    classifier::server s(st, m, p.get<std::string>("tmpdir"));
    
    m->start();
    return start_all(s, p);

  }else if(p.get<std::string>("zookeeper") != ""){
    
    shared_ptr<jubatus::zk,
               pfi::concurrent::threading_model::multi_thread>
      z(new jubatus::zk("localhost:2181", timeout, logfile.c_str()));
    register_actor(*z, name, self, port);

    st.reset(storage::storage_factory::create_storage(p.get<std::string>("storage")));
    classifier::server s(st, p.get<std::string>("tmpdir"));
    return start_all(s, p);
    
  }else
#endif
    {

    st.reset(storage::storage_factory::create_storage(p.get<std::string>("storage")));
    classifier::server s(st, p.get<std::string>("tmpdir"));
    return start_all(s, p);

  }

}
