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

#pragma once

#include <iostream>
#include <sstream>

#include <pficommon/lang/function.h>
#include <pficommon/network/mprpc.h>
#include <pficommon/system/time_util.h>

#include "server_util.hpp"
#include "mixer.hpp"
#include "mixable.hpp"
#include "../common/cht.hpp"
#include "../common/exception.hpp"

using namespace pfi::concurrent;
using pfi::system::time::clock_time;
using pfi::system::time::get_clock_time;

namespace jubatus { namespace framework {

class jubatus_serv : pfi::lang::noncopyable {

public:
  jubatus_serv(const server_argv& a, const std::string& base_path = "/tmp"):
    a_(a),
    update_count_(0),
#ifdef HAVE_ZOOKEEPER_H
    mixer_(new mixer(a_.name, a_.interval_count, a_.interval_sec,
                     pfi::lang::bind(&jubatus_serv::do_mix, this, pfi::lang::_1))),
    use_cht_(false),
#endif
    base_path_(a_.tmpdir)
  {
  };
  virtual ~jubatus_serv(){};

  virtual int start(pfi::network::mprpc::rpc_server& serv){

#ifdef HAVE_ZOOKEEPER_H
    if(! a_.is_standalone()){
      zk_ = pfi::lang::shared_ptr<jubatus::common::lock_service>
	(common::create_lock_service("zk", a_.z, a_.timeout, "logfile_jubatus_serv"));
      ls = zk_;
      jubatus::common::prepare_jubatus(*zk_);

      if( a_.join ){ // join to the existing cluster with -j option
        join_to_cluster(zk_);
      }

      if( use_cht_ ){

        jubatus::common::cht::setup_cht_dir(*zk_, a_.name);
        jubatus::common::cht ht(zk_, a_.name);
        ht.register_node(a_.eth, a_.port);
      }

      mixer_->set_zk(zk_);
      register_actor(*zk_, a_.name, a_.eth, a_.port);
      mixer_->start();
    }
#endif

    { LOG(INFO) << "running in port=" << a_.port; }
    return serv.serv(a_.port, a_.threadnum);
  };

  void register_mixable(mixable0* m){
    mixables_.push_back(m);
  };

  void build_local_path_(std::string& out, const std::string& type, const std::string& id){
    out = base_path_ + "/";
    out += type;
    out += "_";
    out += id;
    out += ".jc";
  };

  pfi::concurrent::rw_mutex& get_rw_mutex(){ return m_; };


  void use_cht(){
#ifdef HAVE_ZOOKEEPER_H
    use_cht_ = true;
#endif
  };

  std::map<std::string, std::map<std::string,std::string> > get_status(int) const {
    std::map<std::string, std::string> data;
    util::get_machine_status(data);

    data["timeout"] = pfi::lang::lexical_cast<std::string>(a_.timeout);
    data["threadnum"] = pfi::lang::lexical_cast<std::string>(a_.threadnum);
    data["tmpdir"] = a_.tmpdir;
    data["interval_sec"] = pfi::lang::lexical_cast<std::string>(a_.interval_sec);
    data["interval_count"] = pfi::lang::lexical_cast<std::string>(a_.interval_count);
    data["is_standalone"] = pfi::lang::lexical_cast<std::string>(a_.is_standalone());
    data["VERSION"] = JUBATUS_VERSION;
    data["PROGNAME"] = JUBATUS_APPNAME;

    data["update_count"] = pfi::lang::lexical_cast<std::string>(update_count_);

#ifdef HAVE_ZOOKEEPER_H
    mixer_->get_status(data);
    data["zk"] = a_.z;
    data["use_cht"] = pfi::lang::lexical_cast<std::string>(use_cht_);
#endif

    std::map<std::string, std::map<std::string,std::string> > ret;
    ret[get_server_identifier()] = data;
    return ret;
  };
  std::string get_server_identifier()const{
    std::stringstream ss;
    ss << a_.eth;
    ss << "_";
    ss << a_.port;
    return ss.str();
  };

#ifdef HAVE_ZOOKEEPER_H
  void join_to_cluster(pfi::lang::shared_ptr<jubatus::common::lock_service> z){
    std::vector<std::string> list;
    std::string path = common::ACTOR_BASE_PATH + "/" + a_.name + "/nodes";
    z->list(path, list);
    if(not list.empty()){
      common::lock_service_mutex zlk(*z, common::ACTOR_BASE_PATH + "/" + a_.name + "/master_lock");
      while(not zlk.try_lock()){ ; }
      size_t i = rand() % list.size();
      std::string ip;
      int port;
      common::revert(list[i], ip, port);
      pfi::network::mprpc::rpc_client c(ip, port, a_.timeout);

      pfi::lang::function<std::string()> f = c.call<std::string()>("get_storage");
      std::stringstream ss( f() );
      for(size_t i = 0;i<mixables_.size(); ++i){
        mixables_[i]->clear();
        mixables_[i]->load(ss);
      }
    }
  };
  
  std::string get_storage(int i){
    std::stringstream ss;
    for(size_t i=0; i<mixables_.size(); ++i){
      mixables_[i]->save(ss);
    }
    return ss.str();
  }

  std::vector<std::string> get_diff_impl(int){
    // if(mixables_.empty()){
    //   //throw config_not_set(); nothing to mix
    // }
    std::vector<std::string> o;
    {
      scoped_lock lk(rlock(m_));
      for(size_t i=0; i<mixables_.size(); ++i){
        o.push_back(mixables_[i]->get_diff());
      }
    }
    return o;
  };
  int put_diff_impl(std::vector<std::string> unpacked){
    scoped_lock lk(wlock(m_));
    if(unpacked.size() != mixables_.size()){
      //deserialization error
    }
    for(size_t i=0; i<mixables_.size(); ++i){
      mixables_[i]->put_diff(unpacked[i]);
    }
    return 0;
  };
  void do_mix(const std::vector<std::pair<std::string,int> >& v);

#endif

  void updated(){
#ifdef HAVE_ZOOKEEPER_H
    update_count_ = mixer_->updated();
#else
    update_count_++;
#endif
  };

  int save(std::string id);
  int load(std::string id);

  // after load( model_ was loaded from file ) called, users reset their own data
  // I'm afraid this function is useless
  virtual void after_load() =0;

  std::string get_ipaddr()const{ return a_.eth; };
  int get_port()const{ return a_.port; };
  int get_threadum()const{ return a_.threadnum; };

protected:
  server_argv a_;
  unsigned int update_count_;

  std::vector<mixable0*> mixables_;
  pfi::lang::shared_ptr<mixer> mixer_;
#ifdef HAVE_ZOOKEEPER_H

  pfi::lang::shared_ptr<jubatus::common::lock_service> zk_;
  bool use_cht_;
#endif

  pfi::concurrent::rw_mutex m_;
  const std::string base_path_;

};

}}

#define JRLOCK__(p) \
  pfi::concurrent::scoped_lock lk(rlock((p)->get_rw_mutex()));

#define JWLOCK__(p) \
  pfi::concurrent::scoped_lock lk(wlock((p)->get_rw_mutex())); \
  p_->updated();
