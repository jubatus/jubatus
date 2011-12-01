#pragma once

#include <iostream>

#include <pficommon/lang/function.h>
#include <pficommon/network/mprpc.h>

#include "server_util.hpp"
#include "mixer.hpp"

using namespace pfi::concurrent;

namespace jubatus { namespace server {

template <typename M, typename Diff>
class jubatus_serv : pfi::lang::noncopyable {

public:
  jubatus_serv(const server_argv& a, const std::string& base_path = "/tmp"):
    is_mixer_func_set_(false),
    a_(a),
    base_path_(base_path_)
  {
  };
  virtual ~jubatus_serv(){};

  int start(){
//#ifdef HAVE_ZOOKEEPER_H
    if(! a_.is_standalone()){
      pfi::lang::shared_ptr<jubatus::zk> z(new jubatus::zk(a_.z, a_.timeout, "log"));

      if( a_.join ){ // join to the existing cluster with -j option
        join_to_cluster(z);
      }

      mixer_.reset(new mixer0<M, Diff>(z, a_.name, a_.interval_count, a_.interval_sec));
      if(is_mixer_func_set_){
        mixer_->start();
      }
    }
//#endif
    // return this->serv(a_.port, a_.threadnum); FIXME
    return -1;
  };


  void build_local_path_(std::string& out, const std::string& type, const std::string& id){
    out = base_path_ + "/";
    out += type;
    out += "_";
    out += id;
    out += ".jc";
  };

  void set_mixer(pfi::lang::function<Diff(const M*)> get_diff, //get_diff
                 pfi::lang::function<int(const M*, const Diff&, Diff&)> reduce, //mix
                 pfi::lang::function<int(M*, const Diff&)> put_diff //put_diff
                 ) {
//#ifdef HAVE_ZOOKEEPER_H
    get_diff_ = get_diff;
    reduce_ = reduce;
    put_diff_ = put_diff;
    mixer_->set_mixer_func(pfi::lang::bind(&jubatus_serv<M,Diff>::do_mix, this, pfi::lang::_1));
    is_mixer_func_set_ = true;
//#endif
  };

//#ifdef HAVE_ZOOKEEPER_H
  void join_to_cluster(pfi::lang::shared_ptr<jubatus::zk> z){
    std::vector<std::string> list;
    std::string path = ACTOR_BASE_PATH + "/" + a_.name + "/nodes";
    z->list(path, list);
    if(not list.empty()){
      zkmutex zlk(z, ACTOR_BASE_PATH + "/" + a_.name + "/master_lock");
      while(not zlk.try_lock()){ ; }
      size_t i = rand() % list.size();
      std::string ip;
      int port;
      revert(list[i], ip, port);
      pfi::network::mprpc::rpc_client c(ip, port, a_.timeout);
      // typename pfi::lang::function<M()> f = c.call<M()>("get_storage");
      // FIXME: if you use this code, M should be serializable
      // classifier used serialized binary 'std::string', not local_storage
      // how do we make this type pattern?
      try{
      // this->set_storage( f() );
      }catch(std::exception& e){
      // if(s.success){
      //   stringstream ss(s.retval);
      //   st->load(ss);
      // }
      }
    }
  };

  pfi::concurrent::rw_mutex& get_rw_mutex(){ return m_; };

  std::string get_diff_impl(int){
    msgpack::sbuffer sbuf;
    scoped_lock lk(rlock(m_));
    msgpack::pack(sbuf, this->get_diff_(model_.get()));
    return std::string(sbuf.data(), sbuf.size());
  };
    int put_diff_impl(std::string d){
    msgpack::unpacked msg;
    msgpack::unpack(&msg, d.c_str(), d.size());
    Diff diff;
    msg.get().convert(&diff);
    scoped_lock lk(wlock(m_));
    return this->put_diff_(model_.get(), diff);
  };
  void do_mix(const std::vector<std::pair<std::string,int> >& v){
    Diff acc;
    typename pfi::lang::function<Diff(int)> get_diff_fun;
    for(size_t s = 0; s < v.size(); ++s ){
      get_diff_fun = pfi::network::mprpc::rpc_client(v[s].first, v[s].second, a_.timeout).call<Diff(int)>("get_diff");
      Diff d = get_diff_fun(0);
      scoped_lock lk(rlock(m_)); // model_ should not be in mix (reduce)?
      this->reduce_(model_.get(), d, acc);
    }
    typename pfi::lang::function<int(Diff)> put_diff_fun;
    for(size_t s = 0; s < v.size(); ++s ){
      put_diff_fun = pfi::network::mprpc::rpc_client(v[s].first, v[s].second, a_.timeout).call<int(Diff)>("put_diff");
      put_diff_fun(acc);
    }
  }

  int save(std::string id) {
    std::string ofile;
    build_local_path_(ofile, model_->type, id);
    
    std::ofstream ofs(ofile.c_str(), std::ios::trunc|std::ios::binary);
    if(!ofs){
      //    return ::fail(ofile + ": cannot open (" + pfi::lang::lexical_cast<std::string>(errno) + ")" );
    }
    try{
      model_->save(ofs);
      ofs.close();
      LOG(INFO) << "saved to " << ofile;
      return 0; //int>::ok(0);
    }catch(const std::exception& e){
      return -1;
    }
  }

  virtual pfi::lang::shared_ptr<M> before_load() =0;
  // after load( model_ was loaded from file ) called, users reset their own data
  virtual void after_load() =0;

  int load(std::string id) {
    std::string ifile;
    bool ok = false;
    build_local_path_(ifile, model_->type, id);
    
    std::ifstream ifs(ifile.c_str(), std::ios::binary);
    if(!ifs){
      //   return result<int>::fail(ifile + ": cannot open (" + pfi::lang::lexical_cast<std::string>(errno) + ")" );
    }
    try{
      pfi::lang::shared_ptr<M> s = this->before_load();
      if(!s){
        //     return result<int>::fail("cannot allocate memory for storage");
      }
      s->load(ifs);
      ifs.close();
      
      if(ok){
        LOG(INFO) << "loaded from " << ifile;
        model_ = s;
        this->after_load();
        return 0;//result<int>::ok(0);
      }
      return -1; //result<int>::fail("failed loading");
    }catch(const std::exception& e){
      //   return result<int>::fail(e.what());
    }
    return -1;
  }

protected:
  pfi::lang::shared_ptr<mixer0<M, Diff> > mixer_;
  pfi::lang::function<Diff(const M*)> get_diff_;
  pfi::lang::function<int(const M*, const Diff&, Diff&)> reduce_;
  pfi::lang::function<int(M*, const Diff&)> put_diff_;
  //#else
  //protected:
//#endif
  bool is_mixer_func_set_;
  pfi::concurrent::rw_mutex m_;
  server_argv a_;
  const std::string base_path_;

  pfi::lang::shared_ptr<M> model_;
};

}}

#define JRLOCK__(p) \
  pfi::concurrent::scoped_lock lk(rlock((p)->get_rw_mutex()));

#define JWLOCK__(p) \
  pfi::concurrent::scoped_lock lk(wlock((p)->get_rw_mutex()));
