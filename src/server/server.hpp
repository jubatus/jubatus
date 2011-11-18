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

#include "../common/rpc_util.hpp"
#include "../common/zk.hpp"

#ifdef HAVE_ZOOKEEPER_H
#include "mixer.hpp"
#endif

#include "mix.hpp"

namespace jubatus {

void m(const std::vector<std::pair<std::string, int> >&) {}

template <typename M>
class server : public pfi::network::mprpc::rpc_server {
 public:

#ifdef HAVE_ZOOKEEPER_H
  typedef pfi::lang::shared_ptr<
   jubatus::zk, pfi::concurrent::threading_model::multi_thread> zk_type;

  server(zk_type zk, const std::string& name)
      : pfi::network::mprpc::rpc_server(0.0),
        mixer_(zk, name, &m) {}
#endif

  server()
      : pfi::network::mprpc::rpc_server(0.0) {}
  

  template <typename D>
  void register_update(std::string name,
                       pfi::lang::function<void(M*, const D&)> u)
  {
    pfi::lang::function<result<int>(std::string, std::vector<D>)> f
        = pfi::lang::bind(&server<M>::template update<D>, this, u, pfi::lang::_1, pfi::lang::_2);
    add(name, f);
  }

  template <typename Q, typename R>
  void register_analysis(std::string name,
                         pfi::lang::function<R(const M*, const Q&)> q) {
    pfi::lang::function<result<std::vector<R> >(std::string, std::vector<Q>)> f
        = pfi::lang::bind(&server<M>::template analysis<Q, R>, this, q, pfi::lang::_1, pfi::lang::_2);
    add(name, f);
  }

#ifdef HAVE_ZOOKEEPER_H
  template <typename D>
  void set_mixer(pfi::lang::function<D(const M*)> d,
                 pfi::lang::function<int(M*, const D&)> m) {
    {
      pfi::lang::function<D()> f
          = pfi::lang::bind(d, &model);
      add("get_diff", f);
    }
    {
      pfi::lang::function<int(D)> f
          = pfi::lang::bind(m, &model, pfi::lang::_1);
      add("mix", f);
    }
    mixer_.set_mixer_func(&mix<D>);
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
    try {
      //mixer_.updated();
    } catch(const std::exception& e) {
      LOG(WARNING) << e.what();
    }
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
  mixer mixer_;
#endif
};


}
