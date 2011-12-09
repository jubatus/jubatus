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

#include "server_util.hpp"
#include "mixer.hpp"
#include "../common/cht.hpp"

using namespace pfi::concurrent;

namespace jubatus { namespace server {

template <typename M, typename Diff>
class jubatus_serv : pfi::lang::noncopyable {

public:
  jubatus_serv(const server_argv& a, const std::string& base_path = "/tmp"):
    is_mixer_func_set_(false),
    a_(a),
#ifdef HAVE_ZOOKEEPER_H
    mixer_(new mixer0<M, Diff>(a_.name, a_.interval_count, a_.interval_sec)),
    use_cht_(false),
#endif
    base_path_(a_.tmpdir)
  {
    //model_ = make_model(); //compiler warns
  };
  virtual ~jubatus_serv(){};

  virtual int start(pfi::network::mprpc::rpc_server& serv){

#ifdef HAVE_ZOOKEEPER_H
    if(! a_.is_standalone()){
      zk_ = pfi::lang::shared_ptr<jubatus::common::lock_service>(common::create_lock_service("zk", a_.z, a_.timeout, "logfile_jubatus_serv"));

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
      if(is_mixer_func_set_){
        mixer_->start();
      }
    }
#endif

    { LOG(INFO) << "running in port=" << a_.port; }
    return serv.serv(a_.port, a_.threadnum);
  };


  void build_local_path_(std::string& out, const std::string& type, const std::string& id){
    out = base_path_ + "/";
    out += type;
    out += "_";
    out += id;
    out += ".jc";
  };

  pfi::concurrent::rw_mutex& get_rw_mutex(){ return m_; };

  void set_mixer(pfi::lang::function<Diff(const M*)> get_diff, //get_diff
                 pfi::lang::function<int(const M*, const Diff&, Diff&)> reduce, //mix
                 pfi::lang::function<int(M*, const Diff&)> put_diff //put_diff
                 ) {
#ifdef HAVE_ZOOKEEPER_H
    if( ! a_.is_standalone() ){
      get_diff_ = get_diff;
      reduce_ = reduce;
      put_diff_ = put_diff;
      mixer_->set_mixer_func(pfi::lang::bind(&jubatus_serv<M,Diff>::do_mix, this, pfi::lang::_1));
      is_mixer_func_set_ = true;
    }
#endif
  };

#ifdef HAVE_ZOOKEEPER_H
  void use_cht(){
    use_cht_ = true;
  };

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

      typename pfi::lang::function<std::string()> f = c.call<std::string()>("get_storage");
      std::stringstream ss( f() );
      model_ = make_model();
      model_->load(ss);
    }
  };

  std::string get_storage(int i){
    std::stringstream ss;
    model_->save(ss);
    return ss.str();
  }

  std::string get_diff_impl(int){
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, this->get_diff_(model_.get()));
    return std::string(sbuf.data(), sbuf.size());
  };
    int put_diff_impl(std::string d){
    msgpack::unpacked msg;
    msgpack::unpack(&msg, d.c_str(), d.size());
    Diff diff;
    msg.get().convert(&diff);
    return this->put_diff_(model_.get(), diff);
  };
  void do_mix(const std::vector<std::pair<std::string,int> >& v){
    if(not is_mixer_func_set_) return;
    Diff acc;
    std::string serialized_diff;
    for(size_t s = 0; s < v.size(); ++s ){
      try{
        pfi::network::mprpc::rpc_client c(v[s].first, v[s].second, a_.timeout);
        pfi::lang::function<std::string(int)> get_diff_fun = c.call<std::string(int)>("get_diff");
        serialized_diff = get_diff_fun(0);
      }catch(std::exception& e){
        LOG(ERROR) << e.what();
        continue;
      }
      Diff diff;
      msgpack::unpacked msg;
      msgpack::unpack(&msg, serialized_diff.c_str(), serialized_diff.size());
      msg.get().convert(&diff);
      scoped_lock lk(rlock(m_)); // model_ should not be in mix (reduce)?
      this->reduce_(model_.get(), diff, acc);
    }
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, acc);
    serialized_diff = std::string(sbuf.data(), sbuf.size());
    for(size_t s = 0; s < v.size(); ++s ){
      try{
        pfi::network::mprpc::rpc_client c(v[s].first, v[s].second, a_.timeout);
        pfi::lang::function<int(std::string)> put_diff_fun = c.call<int(std::string)>("put_diff");
        put_diff_fun(serialized_diff);
      }catch(std::exception& e){
        LOG(ERROR) << e.what();
        continue;
      }
    }
    DLOG(INFO) << "mixed with " << v.size() << " servers";
  }
#endif

  int save(std::string id) {
    std::string ofile;
    build_local_path_(ofile, model_->type, id);
    
    std::ofstream ofs(ofile.c_str(), std::ios::trunc|std::ios::binary);
    if(!ofs){
      throw std::runtime_error(ofile + ": cannot open (" + pfi::lang::lexical_cast<std::string>(errno) + ")" );
    }
    try{
      model_->save(ofs);
      ofs.close();
      LOG(INFO) << "saved to " << ofile;
      return 0;
    }catch(const std::exception& e){
      return -1;
    }
  }

  virtual pfi::lang::shared_ptr<M> make_model() =0;
  // after load( model_ was loaded from file ) called, users reset their own data
  virtual void after_load() =0;

  int load(std::string id) {
    std::string ifile;
    bool ok = false;
    build_local_path_(ifile, model_->type, id);
    
    std::ifstream ifs(ifile.c_str(), std::ios::binary);
    if(!ifs)throw std::runtime_error(ifile + ": cannot open (" + pfi::lang::lexical_cast<std::string>(errno) + ")" );
    try{
      pfi::lang::shared_ptr<M> s = this->make_model();
      if(!s)throw std::runtime_error("cannot allocate memory for storage");
      ok = s->load(ifs);
      int r = errno;
      ifs.close();
      
      if(ok){
        LOG(INFO) << "loaded from " << ifile;
        model_ = s;
        this->after_load();
        return 0;
      }else{
        LOG(ERROR) << strerror(errno);
        return r;
      }
    }catch(const std::exception& e){
      ifs.close();
    }
    return -1; //expected never reaching here.
  }

  std::string get_ipaddr()const{ return a_.eth; };
  int get_port()const{ return a_.port; };
  int get_threadum()const{ return a_.threadnum; };
  
protected:
  bool is_mixer_func_set_;
  server_argv a_;

#ifdef HAVE_ZOOKEEPER_H
  pfi::lang::shared_ptr<mixer0<M, Diff> > mixer_;
  pfi::lang::function<Diff(const M*)> get_diff_;
  pfi::lang::function<int(const M*, const Diff&, Diff&)> reduce_;
  pfi::lang::function<int(M*, const Diff&)> put_diff_;

  pfi::lang::shared_ptr<jubatus::common::lock_service> zk_;
  bool use_cht_;
#endif

  pfi::concurrent::rw_mutex m_;
  const std::string base_path_;

  pfi::lang::shared_ptr<M> model_;

};

}}

#define JRLOCK__(p) \
  pfi::concurrent::scoped_lock lk(rlock((p)->get_rw_mutex()));

#define JWLOCK__(p) \
  pfi::concurrent::scoped_lock lk(wlock((p)->get_rw_mutex()));
