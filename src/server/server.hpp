#pragma once

#include <vector>
#include <string>
#include <functional>

#include <pficommon/lang/function.h>
#include <pficommon/network/mprpc.h>
#include <pficommon/data/serialization.h>
#include <pficommon/lang/bind.h>
#include <pficommon/lang/shared_ptr.h>

#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>

#include <glog/logging.h>

#include "../common/membership.hpp"
#include "../common/exception.hpp"
#include "../common/rpc_util.hpp"
#include "../common/zk.hpp"

#ifdef HAVE_ZOOKEEPER_H
#include "mixer.hpp"
#endif

#include "server_util.hpp"

namespace jubatus {

void m(const std::vector<std::pair<std::string, int> >&) {}

template <typename M, typename Diff>
class server : public pfi::network::mprpc::rpc_server {
 public:

#ifdef HAVE_ZOOKEEPER_H
  typedef pfi::lang::shared_ptr<jubatus::zk> zk_type;
#endif

  server(const server_argv& a)
    : pfi::network::mprpc::rpc_server(a.timeout),
#ifdef HAVE_ZOOKEEPER_H
      mixer_running_(false),
#endif
      a_(a)
  {
  };

  int start(){
#ifdef HAVE_ZOOKEEPER_H
    if(! a_.is_standalone()){
      pfi::lang::shared_ptr<jubatus::zk> z(new jubatus::zk(a_.z, a_.timeout, "log"));

      if( a_.join ){ // join to the existing cluster with -j option
        join_to_cluster(z);
      }

      mixer_.reset(new mixer0<M, Diff>(z, a_.name, a_.interval_count, a_.interval_sec));
      mixer_running_ = true;
      mixer_->start();
    }
#endif
    return this->serv(a_.port, a_.threadnum);
  };

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
  
  template <typename D>
  void register_update(std::string name,
                       pfi::lang::function<void(M*, const D&)> u)
  {
    pfi::lang::function<result<int>(std::string, std::vector<D>)> f
      = pfi::lang::bind(&server<M, Diff>::template update<D>, this, u, pfi::lang::_1, pfi::lang::_2);
    add(name, f);
  }

  template <typename Q, typename R>
  void register_analysis(std::string name,
                         pfi::lang::function<R(const M*, const Q&)> q) {
    pfi::lang::function<result<std::vector<R> >(std::string, std::vector<Q>)> f
      = pfi::lang::bind(&server<M, Diff>::template analysis<Q, R>, this, q, pfi::lang::_1, pfi::lang::_2);
    add(name, f);
  }

#ifdef HAVE_ZOOKEEPER_H
  void set_mixer(pfi::lang::function<Diff(const M*)> get_diff, //get_diff
                 pfi::lang::function<int(const M*, const Diff&, const Diff&, Diff&)> mix, //mix
                 pfi::lang::function<int(M*, const Diff&)> put_diff //put_diff
                 ) {
    if(mixer_running_){ // TODO: check statically second call of start_mixer
      throw std::runtime_error("can't start mixer twice.");
    }
    { //register to msgpack-RPC
      pfi::lang::function<Diff()> f = pfi::lang::bind(get_diff, &model);
      add(jubatus::GET_DIFF, f);
    }
    { //register to msgpack-RPC
      pfi::lang::function<int(Diff)> f = pfi::lang::bind(put_diff, &model, pfi::lang::_1);
      add(jubatus::PUT_DIFF, f);
    }
    { //register join < FIXME:
      //      pfi::lang::function<M()> f(&get_storage);
      //      add("get_storage", f);
    }
    //register to mixer0
    mixer_->set_mixer_func(mix);
  }
#endif

 private:
  template <typename D>
  result<int> update(pfi::lang::function<void(M*, const D&)> fun,
                     const std::string& name,
                     const std::vector<D>& data) {
    pfi::concurrent::scoped_lock lk(rlock(mutex_));
    int success = 0;
    for (size_t i = 0; i < data.size(); ++i) {
      try {
        fun(&model, data[i]);
        ++success;
      } catch(const std::exception& e) {
        LOG(WARNING) << e.what();
      }
    }
#ifdef HAVE_ZOOKEEPER_H
    try {
      mixer_->updated();
    } catch(const std::exception& e) {
      LOG(WARNING) << e.what();
    }
#endif
    return result<int>::ok(success);
  }

  template <typename Q, typename R>
  result<std::vector<R> > analysis(pfi::lang::function<R(const M*, const Q&)> fun,
                                   const std::string& name,
                                   const std::vector<Q>& query) {
    pfi::concurrent::scoped_lock lk(wlock(mutex_));
    std::vector<R> results;
    for (size_t i = 0; i < query.size(); ++i) {
      try {
        R res = fun(&model, query[i]);
        results.push_back(res);
      } catch(const std::exception& e) {
        LOG(WARNING) << e.what();
      }
    }
    return result<std::vector<R> >::ok(results);
  }

 private:
  pfi::concurrent::rw_mutex mutex_;
  M model;
#ifdef HAVE_ZOOKEEPER_H
  bool mixer_running_;
  pfi::lang::shared_ptr<mixer0<M, Diff> > mixer_;
#endif
  const server_argv a_;
};

}
