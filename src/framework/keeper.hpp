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

#pragma once

#include <string>
#include <vector>

#include <glog/logging.h>

#include <pficommon/lang/function.h>
#include <pficommon/lang/bind.h>
#include <pficommon/network/mprpc.h>
#include <pficommon/concurrent/thread.h>

#include "keeper_common.hpp"

#include "server_util.hpp"
#include <glog/logging.h>

#include <msgpack.hpp>
#include <msgpack/rpc/client.h>
#include "../common/mprpc/rpc_mclient.hpp"
#include "../common/mprpc/rpc_server.hpp"
#include "../common/mprpc/exception.hpp"

#include <iostream>

namespace msgpack { namespace rpc {
extern const object TIMEOUT_ERROR;
// FIXME: ugly import... 
// We should replace msgpack-rpc TIMEOUT_ERROR, CONNECT_ERROR ... by
// POSITIVE_INTEGER style error
}};

namespace jubatus {
namespace framework {

class keeper : public keeper_common, jubatus::common::mprpc::rpc_server {
public:
  typedef msgpack::rpc::request request_type;
  typedef std::vector<std::pair<std::string, int> > host_list_type;

public:
  keeper(const keeper_argv& a);
  virtual ~keeper();
  int run();

  template <typename R>
  void register_random(const std::string& method_name) {
    using namespace mp::placeholders;
    mp::function<R(std::string)> f = mp::bind(&keeper::template random_proxy<R>, this, method_name, _1);
    add(method_name, f);
  }
  template <typename R, typename A0> 
  void register_random(const std::string& method_name) {
    using namespace mp::placeholders;
    mp::function<R(std::string,A0)> f = mp::bind(&keeper::template random_proxy<R,A0>, this, method_name, _1, _2);
    add(method_name, f);
  }
  template <typename R, typename A0, typename A1>
  void register_random(const std::string& method_name) {
    using namespace mp::placeholders;
    mp::function<R(std::string,A0,A1)> f = mp::bind(&keeper::template random_proxy<R,A0,A1>, this, method_name, _1, _2, _3);
    add(method_name,f); 
  }
  template <typename R, typename A0, typename A1, typename A2>
  void register_random(const std::string& method_name) {
    using namespace mp::placeholders;
    mp::function<R(std::string,A0,A1,A2)> f = mp::bind(&keeper::template random_proxy<R,A0,A1,A2>, this, method_name, _1, _2, _3, _4);
    add(method_name,f); 
  }

  template <typename R>
  void register_broadcast(std::string method_name,
                          pfi::lang::function<R(R,R)> agg){
    using namespace mp::placeholders;
    mp::function<R(std::string)> f =
      mp::bind(&keeper::template broadcast_proxy<R>, this, method_name, _1, 
               agg);
    add(method_name, f);
  }
  template <typename R, typename A0>
  void register_broadcast(std::string method_name,
                          pfi::lang::function<R(R,R)> agg){
    using namespace mp::placeholders;
    mp::function<R(std::string, A0)> f =
      mp::bind(&keeper::template broadcast_proxy<R, A0>, this, method_name, _1, _2,
               agg);
    add(method_name, f);
  }

  template <int N, typename R>
  void register_cht(std::string method_name, pfi::lang::function<R(R,R)> agg) {
    using namespace mp::placeholders;
    mp::function<R(std::string, std::string)> f =
      mp::bind(&keeper::template cht_proxy<N,R>, this, method_name, _1, _2, agg);
    add(method_name, f);
  }
  template <int N, typename R, typename A0>
  void register_cht(std::string method_name, pfi::lang::function<R(R,R)> agg) {
    using namespace mp::placeholders;
    mp::function<R(std::string, std::string, A0)> f =
      mp::bind(&keeper::template cht_proxy<N,R,A0>, this, method_name, _1, _2, _3, agg);
    add(method_name, f);
  }
  template <int N, typename R, typename A0, typename A1>
  void register_cht(std::string method_name, pfi::lang::function<R(R,R)> agg) {
    using namespace mp::placeholders;
    mp::function<R(std::string, std::string, A0, A1)> f =
      mp::bind(&keeper::template cht_proxy<N,R, A0, A1>, this, method_name, _1, _2, _3, _4, agg);
    add(method_name, f);
  }
  
  // async random method ( arity 0-4 )
  template <typename R>
  void register_async_random( const std::string &method_name ) {
    typedef typename msgpack::type::tuple<std::string> packed_args_type;
    register_async_vrandom_inner<R, packed_args_type>(method_name);
  }

  template <typename R, typename A0>
  void register_async_random( const std::string &method_name ) {
    typedef typename msgpack::type::tuple<std::string, A0> packed_args_type;
    register_async_vrandom_inner<R, packed_args_type>(method_name);
  }

  template <typename R, typename A0, typename A1>
  void register_async_random( const std::string &method_name ) {
    typedef typename msgpack::type::tuple<std::string, A0, A1> packed_args_type;
    register_async_vrandom_inner<R, packed_args_type>(method_name);
  }

  template <typename R, typename A0, typename A1, typename A2>
  void register_async_random( const std::string &method_name ) {
    typedef typename msgpack::type::tuple<std::string, A0, A1, A2> packed_args_type;
    register_async_vrandom_inner<R, packed_args_type>(method_name);
  }

  template <typename R, typename A0, typename A1, typename A2, typename A3>
  void register_async_random( const std::string &method_name ) {
    typedef typename msgpack::type::tuple<std::string, A0, A1, A2, A3> packed_args_type;
    register_async_vrandom_inner<R, packed_args_type>(method_name);
  }

  // async broadcast method ( arity 0-4 )
  template <typename R>
  void register_async_broadcast(const std::string &method_name,
                                 pfi::lang::function<R(R,R)> agg){
    typedef typename msgpack::type::tuple<std::string> packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template <typename R, typename A0>
  void register_async_broadcast(const std::string &method_name,
                                 pfi::lang::function<R(R,R)> agg){
    typedef typename msgpack::type::tuple<std::string, A0> packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template <typename R, typename A0, typename A1>
  void register_async_broadcast(const std::string &method_name,
                                 pfi::lang::function<R(R,R)> agg){
    typedef typename msgpack::type::tuple<std::string, A0, A1> packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template <typename R, typename A0, typename A1, typename A2>
  void register_async_broadcast(const std::string &method_name,
                                 pfi::lang::function<R(R,R)> agg){
    typedef typename msgpack::type::tuple<std::string, A0, A1, A2> packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template <typename R, typename A0, typename A1, typename A2, typename A3>
  void register_async_broadcast(const std::string &method_name,
                                 pfi::lang::function<R(R,R)> agg){
    typedef typename msgpack::type::tuple<std::string, A0, A1, A2, A3> packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  // async cht method ( arity 0-4 )
  template <int N, typename R>
  void register_async_cht(const std::string &method_name,
                           pfi::lang::function<R(R,R)> agg ) {
    typedef typename msgpack::type::tuple<std::string, std::string> packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template <int N, typename R, typename A0>
  void register_async_cht(const std::string &method_name,
                           pfi::lang::function<R(R,R)> agg ) {
    typedef typename msgpack::type::tuple<std::string, std::string, A0> packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template <int N, typename R, typename A0, typename A1>
  void register_async_cht(const std::string &method_name,
                           pfi::lang::function<R(R,R)> agg ) {
    typedef typename msgpack::type::tuple<std::string, std::string, A0, A1> packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template <int N, typename R, typename A0, typename A1, typename A2>
  void register_async_cht(const std::string &method_name,
                           pfi::lang::function<R(R,R)> agg ) {
    typedef typename msgpack::type::tuple<std::string, std::string, A0, A1, A2> packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template <int N, typename R, typename A0, typename A1, typename A2, typename A3>
  void register_async_cht(const std::string &method_name,
                           pfi::lang::function<R(R,R)> agg ) {
    typedef typename msgpack::type::tuple<std::string, std::string, A0, A1, A2, A3> packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

private:
  template <typename R, typename Tuple>
  void register_async_vrandom_inner( const std::string &method_name ) {
    using namespace mp::placeholders;
    typedef typename common::mprpc::async_vmethod<Tuple>::type vfunc_type;

    vfunc_type f =
      mp::bind(&keeper::template random_async_vproxy<R, Tuple>, this,
               /* request */ _1,
               method_name, /* packed_args */ _2 );
    add_async_vmethod<Tuple>(method_name, f);
  }

  template <typename R, typename Tuple>
  void register_async_vbroadcast_inner(const std::string &method_name, 
                                       const pfi::lang::function<R(R,R)> &agg){
    using namespace mp::placeholders;
    typedef typename common::mprpc::async_vmethod<Tuple>::type vfunc_type;

    vfunc_type f =
      mp::bind(&keeper::template broadcast_async_vproxy<R, Tuple>, this,
               /* request */ _1,
               method_name, /* packed_args */ _2, 
               agg);
    add_async_vmethod<Tuple>(method_name, f);
  }

  template <int N, typename R, typename Tuple>
  void register_async_vcht_inner(const std::string &method_name, 
                                 const pfi::lang::function<R(R,R)> &agg){
    using namespace mp::placeholders;
    typedef typename common::mprpc::async_vmethod<Tuple>::type vfunc_type;

    vfunc_type f =
      mp::bind(&keeper::template cht_async_vproxy<N, R, Tuple>, this,
               /* request */ _1,
               method_name, /* packed_args */ _2, 
               agg);
    add_async_vmethod<Tuple>(method_name, f);
  }

private:
  template <typename R>
  R random_proxy(const std::string& method_name, const std::string& name){
    //    {DLOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    const std::pair<std::string, int>& c = list[rng_(list.size())];
    try{
      msgpack::rpc::client cli(c.first, c.second);
      cli.set_timeout(a_.timeout);
      return cli.call(method_name,name).template get<R>();
    }catch(const std::exception& e){
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw;
    }
  }
  template <typename R, typename A>
  R random_proxy(const std::string& method_name, const std::string& name, const A& arg){
    //    {DLOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    const std::pair<std::string, int>& c = list[rng_(list.size())];
    try{
      msgpack::rpc::client cli(c.first, c.second);
      cli.set_timeout(a_.timeout);
      return cli.call(method_name, name, arg).template get<R>();
    }catch(const std::exception& e){
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw;
    }
  }
  template <typename R, typename A0, typename A1>
  R random_proxy(const std::string& method_name, const std::string& name, const A0& a0, const A1& a1){
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    const std::pair<std::string, int>& c = list[rng_(list.size())];
    try{
      msgpack::rpc::client cli(c.first, c.second);
      cli.set_timeout(a_.timeout);
      return cli.call(method_name, name, a0, a1).template get<R>();
    }catch(const std::exception& e){
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw;
    }
  }
  template <typename R, typename A0, typename A1, typename A2>
  R random_proxy(const std::string& method_name, const std::string& name, const A0& a0, const A1& a1, const A2& a2){
    std::vector<std::pair<std::string, int> > list;

    get_members_(name, list);

    const std::pair<std::string, int>& c = list[rng_(list.size())];
    try{
      msgpack::rpc::client cli(c.first, c.second);
      cli.set_timeout(a_.timeout);
      return cli.call(method_name, name, a0, a1, a2).template get<R>();
    }catch(const std::exception& e){
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw;
    }
  }

  //// async version
  template <typename R, typename Tuple>
  void random_async_vproxy( request_type req, 
                           const std::string& method_name, const Tuple& args){
    std::vector<std::pair<std::string, int> > list;
    std::string name = args.template get<0>();
    get_members_(name, list);
    const std::pair<std::string, int>& c = list[rng_(list.size())];

    async_task_loop::template call_apply<R, Tuple>( c.first, c.second,
                                                    method_name, args, a_.timeout,
                                                    req );
  }
  
  //// 

  template <typename R>
  R broadcast_proxy(const std::string& method_name, const std::string& name,
                    pfi::lang::function<R(R,R)>& agg) {
    //    {DLOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    try{
      jubatus::common::mprpc::rpc_mclient c(list, a_.timeout, get_private_session_pool());
      return *(c.call(method_name, name, agg));
    }catch(const std::exception& e){
      LOG(ERROR) << e.what(); // << " from " << c.first << ":" << c.second;
      throw;
    }
  }
  template <typename R, typename A>
  R broadcast_proxy(const std::string& method_name, const std::string& name, const A& arg,
                    pfi::lang::function<R(R,R)>& agg) {
    //    {DLOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    try{
      std::cout << __LINE__ << " name:" << name << " method:" << method_name << std::endl;
      jubatus::common::mprpc::rpc_mclient c(list, a_.timeout, get_private_session_pool());
      return *(c.call(method_name, name, arg, agg));
    }catch(const std::exception& e){
      std::cout << __LINE__ << e.what() << std::endl;
      // LOG(ERROR) << e.what(); // << " from " << c.first << ":" << c.second;
      throw;
    }
  }

  // async task version
  template <typename R, typename Tuple>
  void broadcast_async_vproxy( request_type req,
                               const std::string& method_name, const Tuple& args,
                               pfi::lang::function<R(R,R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    std::string name = args.template get<0>();
    get_members_(name, list);

    async_task_loop::template call_apply<R, Tuple>( list, method_name, args, a_.timeout,
                                                    req, agg );
  }

  template <int N, typename R>
  R cht_proxy(const std::string& method_name, const std::string& name, const std::string& id,
              pfi::lang::function<R(R,R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    get_members_from_cht_(name, id, list, N);

    try{
      jubatus::common::mprpc::rpc_mclient c(list, a_.timeout, get_private_session_pool());
      return *(c.call(method_name, name, id, agg));
    }catch(const std::exception& e){
      LOG(ERROR) << N << " " << e.what(); // << " from " << c.first << ":" << c.second;
      throw;
    }
  }
  template <int N, typename R, typename A0>
  R cht_proxy(const std::string& method_name, const std::string& name, const std::string& id, const A0& arg,
              pfi::lang::function<R(R,R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    get_members_from_cht_(name, id, list, N);

    try{
      jubatus::common::mprpc::rpc_mclient c(list, a_.timeout, get_private_session_pool());
      return *(c.call(method_name, name, id, arg, agg));
    }catch(const std::exception& e){
      LOG(ERROR) << e.what(); // << " from " << c.first << ":" << c.second;
      throw;
    }
  }
  template <int N, typename R, typename A0, typename A1>
  R cht_proxy(const std::string& method_name, const std::string& name, const std::string& id, const A0& a0, const A1& a1,
              pfi::lang::function<R(R,R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    get_members_from_cht_(name, id, list, N);

    try{
      jubatus::common::mprpc::rpc_mclient c(list, a_.timeout, get_private_session_pool());
      return *(c.call(method_name, name, id, a0, a1, agg));
    }catch(const std::exception& e){
      LOG(ERROR) << e.what(); // << " from " << c.first << ":" << c.second;
      throw;
    }
  }

  //// async version
  template <int N, typename R, typename Tuple>
  void cht_async_vproxy( request_type req,
                         const std::string& method_name, const Tuple &args,
                         pfi::lang::function<R(R,R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    std::string name = args.template get<0>();
    std::string id = args.template get<1>();
    get_members_from_cht_(name, id, list, N);

    async_task_loop::template call_apply<R, Tuple>( list, method_name, args, 
                                                    a_.timeout, req, agg );
  }

  // get thread local session-pool
  msgpack::rpc::session_pool* get_private_session_pool() {
    extern __thread msgpack::rpc::session_pool* private_session_pool_;

    if ( !private_session_pool_ ) private_session_pool_ = new msgpack::rpc::session_pool();
    return private_session_pool_;
  }

  // async task
public:
  class async_task_loop;

public:
  template<typename Res>
  class async_task: public mp::enable_shared_from_this<async_task<Res> > {
  public:
    typedef pfi::lang::shared_ptr<Res> result_ptr;
    typedef pfi::lang::function<Res(Res,Res)> reducer_type;

  public:
    async_task( const host_list_type &hosts, const std::string &method_name,
                request_type req, reducer_type reducer = reducer_type() ) : 
      hosts_(hosts),
      method_name_(method_name),
      req_(req), reducer_(reducer),
      running_count_(0), cancelled_(false) {
    }
    virtual ~async_task() {}
    
    void done_one( msgpack::rpc::future f, int future_index ) {
      namespace jcm = jubatus::common::mprpc;

      mp::pthread_scoped_lock( lock_ );

      if ( !cancelled_ ) {
        try {
          done_one_inner(f, future_index );
        } catch (...) {
          // continue process next result when exception thrown.
          // store exception_thrower to list of errors

          errors_.push_back( jcm::rpc_error( hosts_[future_index].first, 
                                             hosts_[future_index].second, 
                                             jubatus::exception::get_current_exception()));
        }
      }

      futures_[future_index] = msgpack::rpc::future();

      --running_count_;
      if ( !cancelled_ && running_count_ <= 0 ) {
        req_.result<Res>( aggregate_results() );
      }
    }

    Res aggregate_results() {
      if ( results_.size() == 0 )
        return Res(); // FIXME: we should raise exception ?

      if ( !reducer_ || results_.size() == 1 ) 
        return *(results_[0]);

      Res tmp_result = *(results_[0]);
      for(size_t i = 1; i < results_.size(); ++i ) {
        tmp_result = reducer_( tmp_result, *(results_[i]));
      }
      return tmp_result;
    }

    bool on_timeout() {
      mp::pthread_scoped_lock( lock_ );
      if ( !cancelled_ ) {
        cancelled_ = true;
        for(size_t i = 0; i < futures_.size(); ++i ) {
          if ( !futures_[i].is_finished() ) futures_[i].cancel();
        }

        req_.error( msgpack::rpc::TIMEOUT_ERROR );
      }
      return true;
    }

    template<typename Args>
    void call_apply( async_task_loop *at_loop, 
                     const std::string &method_name, const Args &args, int timeout_sec ) {
      running_count_ = hosts_.size();
      
      msgpack::rpc::session_pool &pool = at_loop->pool();
      for(size_t i = 0; i < hosts_.size(); ++i) {
        msgpack::rpc::session s = pool.get_session( hosts_[i].first, hosts_[i].second );

        msgpack::rpc::future f = s.call_apply( method_name, args );
        futures_.push_back(f);
        f.attach_callback( mp::bind( &async_task<Res>::done_one,
                                     this->shared_from_this(),
                                     mp::placeholders::_1, i ));
      }

      if ( timeout_sec > 0 ) {
        msgpack::rpc::loop loop = pool.get_loop();
        loop->add_timer( timeout_sec, 0, 
                         mp::bind( &async_task<Res>::on_timeout, this->shared_from_this() ) );
      }
    }
    
  private:
    host_list_type hosts_;
    std::string method_name_;
    request_type req_;
    reducer_type reducer_;

    int running_count_;
    bool cancelled_;

    std::vector<msgpack::rpc::future> futures_;
    std::vector<result_ptr> results_;
    std::vector<jubatus::common::mprpc::rpc_error> errors_;

    mp::pthread_recursive_mutex lock_;

    void done_one_inner( msgpack::rpc::future f, int future_index ) {
      namespace jcm = jubatus::common::mprpc;

      try {
        result_ptr result( new Res(f.get<Res>()) );
        results_.push_back( result );
      }
      JUBATUS_MSGPACKRPC_EXCEPTION_DEFAULT_HANDLER( method_name_ );
    }
  };

  // async task loop
public:
  class async_task_loop : public mp::enable_shared_from_this<async_task_loop> {
  public:
    async_task_loop() {}
    void run() {
      std::cout << "async_task_loop: start!: " << (void*)this << std::endl;

      while( true ) pool_.run_once();
    }

    msgpack::rpc::session_pool &pool() {
      return pool_;
    }

    static async_task_loop* startup() {
      async_task_loop* at_loop =  new async_task_loop();
      pfi::concurrent::thread thr( pfi::lang::bind( &async_task_loop::run, at_loop ) );
      thr.start();

      return at_loop;
    }

    static async_task_loop* get_private_async_task_loop() {
      if ( !private_async_task_loop_ ) private_async_task_loop_ = startup();

      return private_async_task_loop_;
    }

    template <typename Res, typename Args>
    static void call_apply( const host_list_type &hosts,
                            const std::string &method_name, const Args &args, int timeout_sec,
                            request_type req, 
                            typename async_task<Res>::reducer_type reducer = typename async_task<Res>::reducer_type()) {
      mp::shared_ptr<async_task<Res> > task( new async_task<Res>(hosts, method_name, req, reducer) );
      task->call_apply<Args>( get_private_async_task_loop(), method_name, args, timeout_sec);
    }

    template <typename Res, typename Args>
    static void call_apply( const std::string &host, int port,
                            const std::string &method_name, const Args &args, int timeout_sec,
                            request_type req, 
                            typename async_task<Res>::reducer_type reducer = typename async_task<Res>::reducer_type()) {
      host_list_type hosts;
      hosts.push_back( std::make_pair(host, port) );

      mp::shared_ptr<async_task<Res> > task( new async_task<Res>(hosts, method_name, req, reducer) );
      task->call_apply<Args>( get_private_async_task_loop(), method_name, args, timeout_sec);
    }
    
  protected:
    msgpack::rpc::session_pool pool_;

    static __thread async_task_loop* private_async_task_loop_;
  };

};

}
} //namespace jubatus
