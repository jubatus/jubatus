// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "jubatus_serv.hpp"
#include "../common/util.hpp"
#include "../common/cht.hpp"
#include "../common/exception.hpp"
#include "../common/mprpc/rpc_client.hpp"
#include "server_util.hpp"
#include "aggregators.hpp"

#include <fstream>
#include <sstream>
#include <pficommon/system/time_util.h>
#include <pficommon/lang/cast.h>
#include <cassert>

using std::vector;
using std::string;
using pfi::network::mprpc::rpc_client;
using pfi::lang::function;

using pfi::system::time::clock_time;
using pfi::system::time::get_clock_time;

using jubatus::common::mprpc::rpc_result;

namespace jubatus { namespace framework {

jubatus_serv::jubatus_serv(const server_argv& a,
                           const std::string& base_path):
  a_(a),
  update_count_(0),
#ifdef HAVE_ZOOKEEPER_H
  mixer_(new mixer(a_.type, a_.name, a_.interval_count, a_.interval_sec,
		   pfi::lang::bind(&jubatus_serv::do_mix, this, pfi::lang::_1))),
  use_cht_(false),
#endif
  idgen_(a_.is_standalone()),
  base_path_(a_.tmpdir)
{
}

int jubatus_serv::start(pfi::network::mprpc::rpc_server& serv){

#ifdef HAVE_ZOOKEEPER_H
  if(! a_.is_standalone()){

    // FIXME: need to decide the log file
    // without log output file, zkclient outputs to stderr
    std::string logfile = "/tmp/";
    logfile += a_.program_name;
    logfile += ".";
    logfile += pfi::lang::lexical_cast<std::string>(getpid());
    logfile += ".zklog";

    zk_ = common::cshared_ptr<jubatus::common::lock_service>
      (common::create_lock_service("zk", a_.z, a_.timeout, logfile));
    ls = zk_;
    jubatus::common::prepare_jubatus(*zk_, a_.type, a_.name);
    
    std::string counter_path;
    common::build_actor_path(counter_path, a_.type, a_.name);
    idgen_.set_ls(zk_, counter_path);

    if( a_.join ){ // join to the existing cluster with -j option
      LOG(INFO) << "joining to the cluseter " << a_.name;
      join_to_cluster(zk_);
    }
    
    if( use_cht_ ){
      jubatus::common::cht::setup_cht_dir(*zk_, a_.type, a_.name);
      jubatus::common::cht ht(zk_, a_.type, a_.name);
      ht.register_node(a_.eth, a_.port);
    }
    
    // FIXME(rethink): is this sequence correct?
    mixer_->set_zk(zk_);
    register_actor(*zk_, a_.type, a_.name, a_.eth, a_.port);
    mixer_->start();
  }
#endif

  if( serv.serv(a_.port, a_.threadnum) ){
    LOG(INFO) << "running in port=" << a_.port;
    return 0;
  }else{
    LOG(ERROR) << "failed starting server: any process using port " << a_.port << "?";
    return -1;
  }
}
    
void jubatus_serv::register_mixable(mixable0* m){
#ifdef HAVE_ZOOKEEPER_H
  try{
    m->get_diff(); // #22 ensure m is good pointer at process startup
  }catch(const jubatus::config_not_set& e){
  }

#endif
  mixables_.push_back(m);
}
    
void jubatus_serv::use_cht(){
#ifdef HAVE_ZOOKEEPER_H
  use_cht_ = true;
#endif
}

std::map<std::string, std::map<std::string,std::string> > jubatus_serv::get_status() const {
  std::map<std::string, std::string> data;
  util::get_machine_status(data);
  
  data["timeout"] = pfi::lang::lexical_cast<std::string>(a_.timeout);
  data["threadnum"] = pfi::lang::lexical_cast<std::string>(a_.threadnum);
  data["tmpdir"] = a_.tmpdir;
  data["interval_sec"] = pfi::lang::lexical_cast<std::string>(a_.interval_sec);
  data["interval_count"] = pfi::lang::lexical_cast<std::string>(a_.interval_count);
  data["is_standalone"] = pfi::lang::lexical_cast<std::string>(a_.is_standalone());
  data["VERSION"] = JUBATUS_VERSION;
  data["PROGNAME"] = a_.program_name;
  
  data["update_count"] = pfi::lang::lexical_cast<std::string>(update_count_);
  
#ifdef HAVE_ZOOKEEPER_H
  mixer_->get_status(data);
  data["zk"] = a_.z;
  data["use_cht"] = pfi::lang::lexical_cast<std::string>(use_cht_);
#endif
  
  std::map<std::string, std::map<std::string,std::string> > ret;
  ret[get_server_identifier()] = data;
  return ret;
}

std::string jubatus_serv::get_server_identifier()const{
  std::stringstream ss;
  ss << a_.eth;
  ss << "_";
  ss << a_.port;
  return ss.str();
}
    
//here
#ifdef HAVE_ZOOKEEPER_H
void jubatus_serv::join_to_cluster(common::cshared_ptr<jubatus::common::lock_service> z){
  std::vector<std::string> list;
  std::string path;
  common::build_actor_path(path, a_.type, a_.name);
  z->list(path + "/nodes", list);
  if(list.empty()){
    throw JUBATUS_EXCEPTION(not_found(" cluster to join"));
  }

  common::lock_service_mutex zlk(*z, path + "/master_lock");
  while(not zlk.try_lock()){ ; }
  
  std::string ip;
  int port;
  // if you're using even any cht, you should choose from cht. otherwise random.
  if( use_cht_ ){
    common::cht ht(z, a_.type, a_.name);
    std::pair<std::string, int> predecessor = ht.find_predecessor(a_.eth, a_.port);
    ip = predecessor.first;
    port = predecessor.second;
    DLOG(INFO) << ip << " " << port;
  }else{
    size_t i = rand() % list.size();
    common::revert(list[i], ip, port);
  }
  
  pfi::network::mprpc::rpc_client c(ip, port, a_.timeout);
  
  pfi::lang::function<std::string()> f = c.call<std::string()>("get_storage");
  std::string data = f();
  LOG(INFO) << "join to cluster: " << data.size() << " bytes got from " << ip << " " << port;
  std::stringstream ss( data );
  
  for(size_t i = 0;i<mixables_.size(); ++i){
    mixables_[i]->clear();
    mixables_[i]->load(ss);
  }
  DLOG(INFO) << "all data successfully loaded to " << mixables_.size() << " mixables.";
}

std::string jubatus_serv::get_storage(){
  std::stringstream ss;
  for(size_t i=0; i<mixables_.size(); ++i){
    if(mixables_[i] == NULL){
      LOG(ERROR) << i << "th mixable is null";
      throw JUBATUS_EXCEPTION(config_not_set());
    }
    mixables_[i]->save(ss);
  }
  LOG(INFO) << "new server has come. Sending back " << ss.str().size() << " bytes.";
  return ss.str();
}
    
std::vector<std::string> jubatus_serv::get_diff_impl(int){
  std::vector<std::string> o;

  scoped_lock lk(rlock(m_));
  if(mixables_.empty()){
    throw JUBATUS_EXCEPTION(config_not_set()); // nothing to mix
  }
  for(size_t i=0; i<mixables_.size(); ++i){
    o.push_back(mixables_[i]->get_diff());
  }
  return o;
}

int jubatus_serv::put_diff_impl(std::vector<std::string> unpacked){
  scoped_lock lk(wlock(m_));
  if(unpacked.size() != mixables_.size()){
    //deserialization error
    return -1;
  }
  for(size_t i=0; i<mixables_.size(); ++i){
    mixables_[i]->put_diff(unpacked[i]);
  }
  mixer_->clear();
  return 0;
}

std::vector<std::string> jubatus_serv::mix_agg(const std::vector<std::string>& lhs,
 					       const std::vector<std::string>& rhs){
  if(lhs.size() != rhs.size()){
    // TODO: error. log and assertion exit is good.
  }
  std::vector<std::string> ret;
  for(size_t i = 0; i < lhs.size(); ++i){
    std::string tmp = rhs[i];
    mixables_[i]->reduce(lhs[i], tmp);
    ret.push_back(tmp);
  }
  return ret;
}

void jubatus_serv::do_mix(const std::vector<std::pair<std::string,int> >& v){
  vector<string> accs;
  //vector<string> serialized_diffs;
  clock_time start = get_clock_time();

  if(v.empty()){
    LOG(WARNING) << "no other server. ";
    return;
  }else{
    pfi::lang::function<std::vector<std::string>
                        (const std::vector<std::string>, const std::vector<std::string>)>
      f = pfi::lang::bind(&jubatus_serv::mix_agg, this, pfi::lang::_1, pfi::lang::_2);
    common::mprpc::rpc_mclient c(v, a_.timeout);
    try{
      rpc_result<vector<string> > result_accs = c.call("get_diff", 0, f);
      // TODO: output log when result has error
      rpc_result<int> result_put = c.call("put_diff", *result_accs, pfi::lang::function<int(int,int)>(&framework::add<int>));
      // TODO: output log when result has error
    }catch(const std::exception & e){
      LOG(WARNING) << e.what() << " : mix failed";
      return;
    }
  }

  clock_time end = get_clock_time();
  size_t s = 0;
  for(size_t i=0; i<accs.size(); ++i){
    s+=accs[i].size();
  }
  DLOG(INFO) << "mixed with " << v.size() << " servers in " << (double)(end - start) << " secs, "
             << s << " bytes (serialized data) has been put.";
}
#endif

void jubatus_serv::updated(){
#ifdef HAVE_ZOOKEEPER_H
  update_count_ = mixer_->updated();
#else
  update_count_++;
#endif
}

bool jubatus_serv::save(std::string id)  {
  std::string ofile;
  build_local_path_(ofile, "jubatus", id);
  
  std::ofstream ofs(ofile.c_str(), std::ios::trunc|std::ios::binary);
  if(!ofs){
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(ofile + ": cannot open")
        << jubatus::exception::error_errno(errno));
  }
  try{
    for(size_t i=0; i<mixables_.size(); ++i){
      mixables_[i]->save(ofs);
    }
    ofs.close();
    LOG(INFO) << "saved to " << ofile;
    return true;
  }catch(const std::runtime_error& e){
    LOG(ERROR) << e.what();
    throw;
  }
}
    
bool jubatus_serv::load(std::string id) {
  std::string ifile;
  build_local_path_(ifile, "jubatus", id);

  std::ifstream ifs(ifile.c_str(), std::ios::binary);
  if(!ifs){
    build_local_path0_(ifile, "jubatus", id); // FIXME: add tests for this code
    ifs.open(ifile.c_str(), std::ios::binary);
    if(!ifs)
      throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(ifile + ": cannot open")
          << jubatus::exception::error_errno(errno));
  }
  try{
    for(size_t i = 0;i<mixables_.size(); ++i){
      mixables_[i]->clear();
      mixables_[i]->load(ifs);
    }
    ifs.close();
    this->after_load();
    return true;
  }catch(const std::runtime_error& e){
    ifs.close();
    LOG(ERROR) << e.what();
    throw;
  }
}

void jubatus_serv::get_members(std::vector<std::pair<std::string,int> >& ret)
{
  ret.clear();
#ifdef HAVE_ZOOKEEPER_H
  common::get_all_actors(*zk_, a_.type, a_.name, ret);

  if(ret.empty()){
    return;
  }
  try{
    // remove myself
    for(std::vector<std::pair<std::string,int> >::iterator it = ret.begin(); it != ret.end(); it++){
      if(it->first == a_.eth && it->second == a_.port){
        it = ret.erase(it);
        it--;
        continue;
      }
    }
  }catch(...){
    // eliminate the exception "no clients."
  }
#endif

}

void jubatus_serv::find_from_cht(const std::string& key, size_t n,
				 std::vector<std::pair<std::string,int> >& out)
{
  out.clear();
#ifdef HAVE_ZOOKEEPER_H
  common::cht ht(zk_, a_.type, a_.name);
  ht.find(key, out, n); //replication number of local_node
#else
  //cannot reach here, assertion!
  assert(a_.is_standalone());
  //out.push_back(make_pair(a_.eth, a_.port));
#endif

}

void jubatus_serv::build_local_path_(std::string& out, const std::string& type, const std::string& id) const
{
  out = base_path_ + "/" + a_.eth + "_" + pfi::lang::lexical_cast<std::string>(a_.port) + "_";
  out += type + "_" + id + ".jc";
}

void jubatus_serv::build_local_path0_(std::string& out, const std::string& type, const std::string& id) const
{
  out = base_path_ + "/";
  out += type + "_" + id + ".jc";
}

}}
