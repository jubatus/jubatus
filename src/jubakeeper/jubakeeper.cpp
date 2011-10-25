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

#include <pficommon/concurrent/lock.h>
#include <pficommon/network/mprpc/exception.h>
#include <glog/logging.h>

#include "jubakeeper.hpp"
#include "../common/util.hpp"
#include "../common/membership.hpp"
#include "../server/classifier_rpc.hpp"

using namespace jubatus;
using namespace pfi::lang;
using pfi::concurrent::scoped_lock;
using pfi::network::mprpc::rpc_error;
using std::string;
using std::vector;

jubakeeper::jubakeeper(const std::string& hosts, int port, int max, const std::string& logfile):
  zk_(hosts, 10, logfile)
{
  zk_.create(JUBATUS_BASE_PATH, "");
  zk_.create(JUBAKEEPER_BASE_PATH, "");
  zk_.create(ACTOR_BASE_PATH, "");

  build_existence_path(JUBAKEEPER_BASE_PATH, util::get_ip("eth0"), port, name_);

  zk_.create(name_, "", true);
  // TODO: die when this file deleted by peer

  pfi::lang::function<void()> g = bind(&exit, -1);
  jubatus::push_cleanup(zk_, g);
}
jubakeeper::jubakeeper():
  zk_(""), timeout_(10)
{}
jubakeeper::~jubakeeper(){
}


// str : "<type>/<name>"
// type : "classifier" ...
// name : any but ""
// -> 
result<std::vector<connection_info> > jubakeeper::lookup(std::string str){
  string name = str.substr(str.find_last_of("/")+1);
  vector<std::pair<std::string, int> > ret;
  try{
    get_members_(name, ret);
    return result<std::vector<connection_info> >::ok(ret);
  }catch(const std::exception& e){
    return result<std::vector<connection_info> >::fail(e.what());
  }
}

result<std::vector<connection_info> > jubakeeper::lookup_cht(std::string str, std::string key){
  string name = str.substr(str.find_last_of("/")+1);
  vector<string> list;
  string path = ACTOR_BASE_PATH + "/" + name + "/nodes";

  vector<std::pair<std::string, int> > ret;
  try{
    scoped_lock lk(m_);
    cht c(zk_, name);
    c.find(key, ret);
    return result<std::vector<connection_info> >::ok(ret);
  }catch(const std::exception& e){
    return result<std::vector<connection_info> >::fail(e.what());
  }
}

result<int> jubakeeper::set_config(std::string name, classifier::config_data config){
  std::vector<connection_info> list;
  try{
    get_members_(name, list);
    for(size_t i=0; i<list.size();++i){
      classifier::mprpc_client(list[i].first, list[i].second, timeout_).call_set_config(name, config);
    }
  }catch(const rpc_error& e){
    throw e; // TODO: check ZK and find is he really dead
  }catch(const std::exception& e){  // only one error makes the request fail
    return result<int>::fail(e.what());
  }
  return result<int>::ok(0);
}
result<classifier::config_data> jubakeeper::get_config(std::string name){
  std::vector<std::pair<std::string, int> > list;
  get_members_(name, list);
  classifier::config_data ret;
  try{
    for(size_t i=0; i<list.size();++i){
      result<classifier::config_data> tmp;
      tmp = classifier::mprpc_client(list[i].first, list[i].second, timeout_).call_get_config(name);

      // if(tmp.retval != ret)  // FIXME: do we check all configuration are the same?
      if(!tmp.success)continue;
      else return tmp;
    }
    return result<classifier::config_data>::fail("no server alive");
  }catch(const rpc_error& e){
    throw e; // TODO: check ZK and find is he really dead
  }catch(const std::exception& e){  // only one error makes the request fail
    return result<classifier::config_data>::fail(e.what());
  }
}

result<int> jubakeeper::train(std::string name,std::vector<std::pair<std::string, datum> > data){
  //forward to server
  std::vector<connection_info> list;
  get_members_(name, list);
  connection_info c = list[rng_(list.size())];
  try{
    return classifier::mprpc_client(c.first, c.second, timeout_).call_train(name, data);

  }catch(const rpc_error& e){
    throw e; // TODO: check ZK and find is he really dead, or retry?
  }catch(const std::exception& e){  // only one error makes the request fail
    return result<int>::fail(e.what());
  }
}

result<std::vector<classifier::estimate_results> > jubakeeper::classify(std::string name, std::vector<datum> data){
  //forward to server
  std::vector<connection_info> list;
  get_members_(name, list);
  connection_info c = list[rng_(list.size())];
  try{
    return classifier::mprpc_client(c.first, c.second, timeout_).call_classify(name, data);

  }catch(const rpc_error& e){
    throw e; // TODO: check ZK and find is he really dead, or retry?
  }catch(const std::exception& e){  // only one error makes the request fail
    return result<std::vector<classifier::estimate_results> >::fail(e.what());
  }
}

result<int> jubakeeper::save(std::string name, std::string type, std::string id){
  std::vector<std::pair<std::string, int> > list;

  try{
    get_members_(name, list);
    for(size_t i=0; i<list.size();++i){
      result<int> r = classifier::mprpc_client(list[i].first, list[i].second, timeout_).call_save(name,type,id); 
      if(!r.success){
        return result<int>::fail(list[i].first+"_"+pfi::lang::lexical_cast<std::string>(list[i].second) + ": " + r.error);
      }
    }
    return result<int>::ok(0);
  }catch(const rpc_error& e){
    throw e; // TODO: check ZK and find is he really dead
  }catch(const std::exception& e){
    return result<int>::fail(e.what());
  }
}

result<int> jubakeeper::load(std::string name, std::string type, std::string id){
  std::vector<std::pair<std::string, int> > list;
  get_members_(name, list);
  try{
    for(size_t i=0; i<list.size();++i){
      result<int> r = classifier::mprpc_client(list[i].first, list[i].second, timeout_).call_load(name,type,id); 
      if(!r.success){
        return result<int>::fail(list[i].first+"_"+pfi::lang::lexical_cast<std::string>(list[i].second) + ": " + r.error);
      }
    }
    return result<int>::ok(0);
  }catch(const rpc_error& e){
    throw e; // TODO: check ZK and find is he really dead
  }catch(const std::exception& e){
    return result<int>::fail(e.what());
  }
}
result<std::map<connection_info, std::map<std::string, std::string> > >  jubakeeper::get_status(std::string name){
  std::vector<std::pair<std::string, int> > list;
  get_members_(name, list);
  std::map<std::pair<std::string, int>, std::map<std::string, std::string> > ret;
  try{
    result<std::map<std::pair<std::string,int>,std::map<std::string, std::string> > > tmp;
    for(size_t i=0; i<list.size();++i){
      tmp = classifier::mprpc_client(list[i].first, list[i].second, timeout_).call_get_status(name);
      
      if(tmp.success){ // FIXME: on get_status() we'd like to return both retval and error to client.
	ret.insert(*(tmp.retval.begin())); //FIXME if tmp.retval is empty
      }else{
	return result<std::map<connection_info, std::map<std::string, std::string> > >::fail(tmp.error);
      }
    }
  }catch(const rpc_error& e){
    throw e; // TODO: check ZK and find is he really dead
  }catch(const std::exception& e){
    return result<std::map<connection_info, std::map<std::string, std::string> > >::fail(e.what());
  }
  return result<std::map<connection_info, std::map<std::string, std::string> > >::ok(ret);
}


// TODO: need cache? but status changes.
void jubakeeper::get_members_(const std::string& name, std::vector<std::pair<std::string, int> >& ret){
  ret.clear();
  vector<string> list;
  string path = ACTOR_BASE_PATH + "/" + name + "/nodes";

  {
    scoped_lock lk(m_);
    zk_.list(path, list);
  }
  vector<string>::const_iterator it;

  // FIXME:
  // do you return all server list? it can be very large
  for(it = list.begin(); it!= list.end(); ++it){
    string ip;
    int port;
    jubatus::revert(*it, ip, port);
    ret.push_back(make_pair(ip,port));
  }
}
