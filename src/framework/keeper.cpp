// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "keeper.hpp"

#include <glog/logging.h>

#include "../common/membership.hpp"
#include "../common/exception.hpp"
#include "server_util.hpp"

using namespace jubatus;
using namespace jubatus::framework;

keeper::keeper(const keeper_argv& a)
  : pfi::network::mprpc::rpc_server(a.timeout),
    a_(a),
    zk_(common::create_lock_service("cached_zk", a.z, a.timeout))
    //    zk_(common::create_lock_service("zk", a.z, a.timeout))
{
  ls = zk_;
  jubatus::common::prepare_jubatus(*zk_);
  if(!register_keeper(*zk_, a_.eth, a_.port) ){
    throw membership_error("can't register to zookeeper.");
  }
  register_broadcast_analysis<int, std::string>("save");
  register_broadcast_update<std::string>("load");
}

keeper::~keeper(){
}

int keeper::run(){
  { LOG(INFO) << "running in port=" << a_.port; }
  set_exit_on_term();
  return this->serv(a_.port, a_.threadnum);
};

void keeper::get_members_(const std::string& name, std::vector<std::pair<std::string, int> >& ret){
  using namespace std;
  ret.clear();
  vector<string> list;
  string path = common::ACTOR_BASE_PATH + "/" + name + "/nodes";

  {
    pfi::concurrent::scoped_lock lk(mutex_);
    zk_->list(path, list);
  }
  vector<string>::const_iterator it;
  
  // FIXME:
  // do you return all server list? it can be very large
  for(it = list.begin(); it!= list.end(); ++it){
    string ip;
    int port;
    common::revert(*it, ip, port);
    ret.push_back(make_pair(ip,port));
  }
}
