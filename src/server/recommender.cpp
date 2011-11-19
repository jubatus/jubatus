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
#include "../common/cmdline.h"
#include "recommender_serv.hpp"
#include "../common/util.hpp"
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

static const string PROGNAME(JUBATUS_APPNAME "_recommender");
static const string VERSION(JUBATUS_VERSION);

int start(recommender::server& s, const cmdline::parser& p);

int main(int argc, char *argv[])
{
  signal(SIGPIPE, SIG_IGN);

  cmdline::parser p;
  p.add<int>("rpc-port", 'p', "port number", false, 9198);
  p.add<int>("thread", 'c', "thread number", false, 2);
  p.add<int>("timeout", 't', "time out", false, 10);

  p.add<std::string>("zookeeper", 'z', "zookeeper location", false);
  p.add<std::string>("name", 'n', "learning machine instance name", true);
  p.add<std::string>("tmpdir", 'd', "directory to place plugins", false, "/tmp");

  google::InitGoogleLogging(argv[0]);  
  //  google::LogToStderr(); // only when debug

  p.parse_check(argc, argv);

  std::string self(jubatus::util::get_ip("eth0"));
  std::string name(p.get<std::string>("name"));

  LOG(INFO) << "starting " << PROGNAME << " " << VERSION;
  LOG(INFO) << "mprpc-server at " << self << ":" << p.get<int>("rpc-port");

  bool is_standalone = (p.get<std::string>("zookeeper") == "");
  if(is_standalone){
    jubatus::recommender::server s(p.get<std::string>("tmpdir"));
    LOG(INFO) << "starting in standalone mode.";
    int r = start(s, p);
    LOG(ERROR) << "come " << r << endl;
    return r;
  }
#ifdef HAVE_ZOOKEEPER_H

  else{
    int port = p.get<int>("rpc-port");
    shared_ptr<jubatus::zk> z(new jubatus::zk(p.get<std::string>("zookeeper")));
    shared_ptr<mixer> m(new mixer(z, name, &mixer::dummy));
    recommender::server s(m, p.get<std::string>("tmpdir"));
    jubatus::cht c(*z, name);

    // get anchor graph from neighbor
    vector<std::pair<string, int> > list;
    c.find(self, port, list);

    // if(!list.empty()){
    //   zkmutex zlk(z, ACTOR_BASE_PATH + "/" + name + "/master_lock");
    //   while(not zlk.try_lock()){ ; }
    //   recommender::mprpc_client c(list[0].first, list[0].second, p.get<int>("timeout"));
    //   recommender::rows r = c.call_get_all_rows();
    //   s.update_row(r);
    // }

    c.register_node(self, port);
    register_actor(*z, name, self, p.get<int>("rpc-port"));

    m->start();
    LOG(INFO) << "starting in cluster mode.";
    return start(s, p);
  }
#endif

}

int start( jubatus::recommender::server& s, const cmdline::parser& p){
  jubatus::recommender::mprpc_server serv(p.get<int>("timeout"));
  s.bind_all_methods(serv, util::get_ip("eth0"), p.get<int>("rpc-port"));
  if(!serv.serv(p.get<int>("rpc-port"), p.get<int>("thread")))
    LOG(ERROR) << "failed to start server.";
  return 0;
}
