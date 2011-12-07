#include "keeper.hpp"
#include "../common/membership.hpp"
#include "../common/exception.hpp"

using namespace jubatus;
using namespace jubatus::server;

keeper::keeper(const jubatus::keeper_argv& a)
  : pfi::network::mprpc::rpc_server(a.timeout),
    a_(a),
    zk_(a.z, a.timeout)
{
  if(!register_keeper(zk_, a_.eth, a_.port) ){
    throw membership_error("can't register to zookeeper.");
  }
  register_broadcast_analysis<int, std::string>("save");
  register_broadcast_update<std::string>("load");
}

keeper::~keeper(){
}

int keeper::run(){
  { LOG(INFO) << "running in port=" << a_.port; }
  return this->serv(a_.port, a_.threadnum);
};

