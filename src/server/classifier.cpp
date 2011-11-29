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
#include <signal.h>

#include <glog/logging.h>

#include "../common/membership.hpp"
#include "../storage/storage_factory.hpp"
#include "classifier_serv.hpp"
#include "classifier_rpc.hpp"

using namespace std;
using namespace pfi::lang;
using namespace jubatus;

SET_PROGNAME("classifier");

int start_all(classifier::server &s, const jubatus::server_argv& a){
  classifier::mprpc_server serv(a.timeout);
  {  // run msgpack-rpc server
    s.bind_all_methods(serv, a.eth, a.port);
    if(!serv.serv(a.port, a.threadnum)){
      LOG(ERROR) << "failed starting server";
      return -1;
    }
  }
  return 0;
}

int main(int argc, char *argv[])
{
  signal(SIGPIPE, SIG_IGN);
  jubatus::server_argv a(argc, argv);
  google::InitGoogleLogging(argv[0]);
  //  google::LogToStderr(); // only when debug

  LOG(INFO) << a.boot_message(PROGNAME);
  shared_ptr<storage::storage_base> st;
  
#ifdef HAVE_ZOOKEEPER_H
  std::string logfile = a.tmpdir + "/" + PROGNAME + ".zklog";

  // running on multiple server mode
  if(a.z != "" and a.name != ""){
    std::string storage = "local_mixture";
    shared_ptr<jubatus::zk>  z(new jubatus::zk(a.z, a.timeout, logfile.c_str()));
    st.reset(storage::storage_factory::create_storage(storage));

    if(a.join){
      vector<string> list;
      string path = ACTOR_BASE_PATH + "/" + a.name + "/nodes";
      z->list(path, list);
      if(not list.empty()){
	zkmutex zlk(z, ACTOR_BASE_PATH + "/" + a.name + "/master_lock");
	while(not zlk.try_lock()){ ; }
	size_t i = rand() % list.size();
	string ip;
	int port;
	revert(list[i], ip, port);
	classifier::mprpc_client c(ip, port, a.timeout);
	result<string> s = c.call_get_storage(0);
	if(s.success){
	  stringstream ss(s.retval);
	  st->load(ss);
	}
      }
    }
    register_actor(*z, a.name, a.eth, a.port);
      
    shared_ptr<mixer>  m(new mixer(z, a.name));
    classifier::server s(st, a, a.tmpdir);
    s.set_mixer(m);

    m->start();
    return start_all(s, a);
  }
#endif

  {  // standalone mode with 
    st.reset(storage::storage_factory::create_storage("local"));
    classifier::server s(st, a, a.tmpdir);
    return start_all(s, a);
  }
}
