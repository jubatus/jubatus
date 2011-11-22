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

#include <signal.h>

#include "recommender_serv.hpp"
#include "server_util.hpp"

#ifdef HAVE_ZOOKEEPER_H
#  include "../common/membership.hpp"
#  include "../common/zk.hpp"
#  include "../common/cht.hpp"

#  include "mixer.hpp"
#endif

#include <glog/logging.h>

using namespace std;
using namespace pfi::lang;
using namespace jubatus;

SET_PROGNAME("recommender");

int start(recommender::server& s, const jubatus::server_argv& a){
  jubatus::recommender::mprpc_server serv(a.timeout);
  s.bind_all_methods(serv, a.eth, a.port);
  if(!serv.serv(a.port, a.threadnum))
    LOG(ERROR) << "failed to start server.";
  return 0;

}

int main(int argc, char *argv[])
{
  signal(SIGPIPE, SIG_IGN);
  jubatus::server_argv a(argc, argv);

  google::InitGoogleLogging(argv[0]);  
  //  google::LogToStderr(); // only when debug

  LOG(INFO) << a.boot_message(PROGNAME);

  bool is_standalone = (a.z == "");
  if(is_standalone){
    jubatus::recommender::server s(a.tmpdir);
    LOG(INFO) << "starting in standalone mode.";
    int r = start(s, a);
    LOG(ERROR) << "come " << r << endl;
    return r;
  }
#ifdef HAVE_ZOOKEEPER_H

  else{
    shared_ptr<jubatus::zk> z(new jubatus::zk(a.z));
    shared_ptr<mixer> m(new mixer(z, a.name, &mixer::dummy));
    recommender::server s(m, a.tmpdir);
    jubatus::cht c(*z, a.name);

    // get anchor graph from neighbor
    if(a.join){
      vector<std::pair<string, int> > list;
      c.find(a.eth, a.port, list);
      
      if(!list.empty()){
	zkmutex zlk(z, ACTOR_BASE_PATH + "/" + a.name + "/master_lock");
	while(not zlk.try_lock()){ ; }
	recommender::mprpc_client cli(list[0].first, list[0].second, a.timeout);
	result<recommender::rows> r = cli.call_get_all_rows(a.name);
	s.update_row(a.name, r.retval);
      }
    }
    c.register_node(a.eth, a.port);
    register_actor(*z, a.name, a.eth, a.port);
    
    m->start();
    LOG(INFO) << "starting in cluster mode.";
    return start(s, a);
  }
#endif
}
