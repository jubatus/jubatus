// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_SERVER_FRAMEWORK_KEEPER_HPP_
#define JUBATUS_SERVER_FRAMEWORK_KEEPER_HPP_

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <glog/logging.h>
#include <jubatus/msgpack/rpc/client.h>
#include <msgpack.hpp>
#include <pficommon/concurrent/thread.h>
#include <pficommon/lang/function.h>
#include <pficommon/lang/bind.h>

#include "keeper_common.hpp"
#include "server_util.hpp"
#include "../common/mprpc/rpc_mclient.hpp"
#include "../common/mprpc/rpc_server.hpp"
#include "../common/mprpc/exception.hpp"

namespace msgpack {
namespace rpc {
extern const object TIMEOUT_ERROR;
// TODO(kumagi): ugly import...
// We should replace msgpack-rpc TIMEOUT_ERROR, CONNECT_ERROR ... by
// POSITIVE_INTEGER style error
}  // namespace rpc
}  // namespace msgpack

namespace jubatus {
namespace server {
namespace framework {

class keeper
    : public keeper_common, jubatus::server::common::mprpc::rpc_server {
 public:
  typedef msgpack::rpc::request request_type;
  typedef std::vector<std::pair<std::string, int> > host_list_type;

 public:
  explicit keeper(const keeper_argv& a);
  virtual ~keeper();

  int run();

  template<typename R>
  void register_random(const std::string& method_name) {
    using mp::placeholders::_1;
    mp::function<R(std::string)> f = mp::bind(
        &keeper::template random_proxy0<R>, this, method_name, _1);
    add(method_name, f);
  }

  template<typename R, typename A0>
  void register_random(const std::string& method_name) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    mp::function<R(std::string, A0)> f = mp::bind(
        &keeper::template random_proxy1<R, A0>, this, method_name, _1, _2);
    add(method_name, f);
  }

  template<typename R, typename A0, typename A1>
  void register_random(const std::string& method_name) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    using mp::placeholders::_3;
    mp::function<R(std::string, A0, A1)> f = mp::bind(
        &keeper::template random_proxy2<R, A0, A1>,
        this, method_name, _1, _2, _3);
    add(method_name, f);
  }

  template<typename R, typename A0, typename A1, typename A2>
  void register_random(const std::string& method_name) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    using mp::placeholders::_3;
    using mp::placeholders::_4;
    mp::function<R(std::string, A0, A1, A2)> f = mp::bind(
        &keeper::template random_proxy3<R, A0, A1, A2>,
        this, method_name, _1, _2, _3, _4);
    add(method_name, f);
  }

  template<typename R>
  void register_broadcast(std::string method_name,
                          pfi::lang::function<R(R, R)> agg) {
    using mp::placeholders::_1;
    mp::function<R(std::string)> f = mp::bind(
        &keeper::template broadcast_proxy0<R>, this, method_name, _1, agg);
    add(method_name, f);
  }

  template<typename R, typename A0>
  void register_broadcast(std::string method_name,
                          pfi::lang::function<R(R, R)> agg) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    mp::function<R(std::string, A0)> f = mp::bind(
        &keeper::template broadcast_proxy1<R, A0>,
        this, method_name, _1, _2, agg);
    add(method_name, f);
  }

  template<int N, typename R>
  void register_cht(std::string method_name, pfi::lang::function<R(R, R)> agg) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    mp::function<R(std::string, std::string)> f = mp::bind(
        &keeper::template cht_proxy0<N, R>, this, method_name, _1, _2, agg);
    add(method_name, f);
  }

  template<int N, typename R, typename A0>
  void register_cht(std::string method_name, pfi::lang::function<R(R, R)> agg) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    using mp::placeholders::_3;
    mp::function<R(std::string, std::string, A0)> f = mp::bind(
        &keeper::template cht_proxy1<N, R, A0>,
        this, method_name, _1, _2, _3, agg);
    add(method_name, f);
  }

  template<int N, typename R, typename A0, typename A1>
  void register_cht(std::string method_name, pfi::lang::function<R(R, R)> agg) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    using mp::placeholders::_3;
    using mp::placeholders::_4;
    mp::function<R(std::string, std::string, A0, A1)> f = mp::bind(
        &keeper::template cht_proxy2<N, R, A0, A1>,
        this, method_name, _1, _2, _3, _4, agg);
    add(method_name, f);
  }

  // async random method ( arity 0-4 )
  template<typename R>
  void register_async_random(const std::string& method_name) {
    typedef typename msgpack::type::tuple<std::string> packed_args_type;
    register_async_vrandom_inner<R, packed_args_type>(method_name);
  }

  template<typename R, typename A0>
  void register_async_random(const std::string& method_name) {
    typedef typename msgpack::type::tuple<std::string, A0> packed_args_type;
    register_async_vrandom_inner<R, packed_args_type>(method_name);
  }

  template<typename R, typename A0, typename A1>
  void register_async_random(const std::string& method_name) {
    typedef typename msgpack::type::tuple<std::string, A0, A1>
      packed_args_type;
    register_async_vrandom_inner<R, packed_args_type>(method_name);
  }

  template<typename R, typename A0, typename A1, typename A2>
  void register_async_random(const std::string& method_name) {
    typedef typename msgpack::type::tuple<std::string, A0, A1, A2>
      packed_args_type;
    register_async_vrandom_inner<R, packed_args_type>(method_name);
  }

  template<typename R, typename A0, typename A1, typename A2, typename A3>
  void register_async_random(const std::string& method_name) {
    typedef typename msgpack::type::tuple<std::string, A0, A1, A2, A3>
      packed_args_type;
    register_async_vrandom_inner<R, packed_args_type>(method_name);
  }

  // async broadcast method ( arity 0-4 )
  template<typename R>
  void register_async_broadcast(
      const std::string& method_name,
      pfi::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string> packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template<typename R, typename A0>
  void register_async_broadcast(
      const std::string& method_name,
      pfi::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, A0> packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template<typename R, typename A0, typename A1>
  void register_async_broadcast(
      const std::string& method_name,
      pfi::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, A0, A1> packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template<typename R, typename A0, typename A1, typename A2>
  void register_async_broadcast(
      const std::string& method_name,
      pfi::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, A0, A1, A2>
      packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template<typename R, typename A0, typename A1, typename A2, typename A3>
  void register_async_broadcast(
      const std::string& method_name,
      pfi::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, A0, A1, A2, A3>
      packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  // async cht method ( arity 0-4 )
  template<int N, typename R>
  void register_async_cht(
      const std::string& method_name,
      pfi::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, std::string>
      packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template<int N, typename R, typename A0>
  void register_async_cht(
      const std::string& method_name,
      pfi::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, std::string, A0>
      packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template<int N, typename R, typename A0, typename A1>
  void register_async_cht(
      const std::string& method_name,
      pfi::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, std::string, A0, A1>
      packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template<int N, typename R, typename A0, typename A1, typename A2>
  void register_async_cht(
      const std::string& method_name,
      pfi::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, std::string, A0, A1, A2>
      packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template<int N, typename R, typename A0, typename A1, typename A2,
           typename A3>
  void register_async_cht(
      const std::string& method_name,
      pfi::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, std::string, A0, A1, A2,
        A3> packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

 private:
  template<typename R, typename Tuple>
  void register_async_vrandom_inner(const std::string& method_name) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    typedef typename common::mprpc::async_vmethod<Tuple>::type vfunc_type;

    vfunc_type f = mp::bind(
        &keeper::template random_async_vproxy<R, Tuple>,
        this, /* request */_1, method_name, /* packed_args */_2);
    add_async_vmethod<Tuple>(method_name, f);
  }

  template<typename R, typename Tuple>
  void register_async_vbroadcast_inner(
      const std::string& method_name,
      const pfi::lang::function<R(R, R)>& agg) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    typedef typename common::mprpc::async_vmethod<Tuple>::type vfunc_type;

    vfunc_type f = mp::bind(
        &keeper::template broadcast_async_vproxy<R, Tuple>,
        this, /* request */_1, method_name, /* packed_args */_2, agg);
    add_async_vmethod<Tuple>(method_name, f);
  }

  template<int N, typename R, typename Tuple>
  void register_async_vcht_inner(
      const std::string& method_name,
      const pfi::lang::function<R(R, R)>& agg) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    typedef typename common::mprpc::async_vmethod<Tuple>::type vfunc_type;

    vfunc_type f = mp::bind(
        &keeper::template cht_async_vproxy<N, R, Tuple>,
        this, /* request */_1, method_name, /* packed_args */_2, agg);
    add_async_vmethod<Tuple>(method_name, f);
  }

 private:
  template<typename R>
  R random_proxy0(const std::string& method_name, const std::string& name) {
    DLOG(INFO) << __func__ << " " << method_name << " " << name;
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    const std::pair<std::string, int>& c = list[rng_(list.size())];
    DLOG(INFO) << "request to " << c.first << ":" << c.second;

    try {
      msgpack::rpc::client cli(c.first, c.second);
      cli.set_timeout(a_.timeout);
      return cli.call(method_name, name).template get<R>();
    } catch (const std::exception& e) {
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw;
    }
  }

  template<typename R, typename A>
  R random_proxy1(
      const std::string& method_name,
      const std::string& name,
      const A& arg) {
    DLOG(INFO) << __func__ << " " << method_name << " " << name;
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    const std::pair<std::string, int>& c = list[rng_(list.size())];
    DLOG(INFO) << "request to " << c.first << ":" << c.second;

    try {
      msgpack::rpc::client cli(c.first, c.second);
      cli.set_timeout(a_.timeout);
      return cli.call(method_name, name, arg).template get<R>();
    } catch (const std::exception& e) {
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw;
    }
  }

  template<typename R, typename A0, typename A1>
  R random_proxy2(
      const std::string& method_name,
      const std::string& name,
      const A0& a0,
      const A1& a1) {
    DLOG(INFO) << __func__ << " " << method_name << " " << name;
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    const std::pair<std::string, int>& c = list[rng_(list.size())];
    DLOG(INFO) << "request to " << c.first << ":" << c.second;

    try {
      msgpack::rpc::client cli(c.first, c.second);
      cli.set_timeout(a_.timeout);
      return cli.call(method_name, name, a0, a1).template get<R>();
    } catch (const std::exception& e) {
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw;
    }
  }

  template<typename R, typename A0, typename A1, typename A2>
  R random_proxy3(
      const std::string& method_name,
      const std::string& name,
      const A0& a0,
      const A1& a1,
      const A2& a2) {
    DLOG(INFO) << __func__ << " " << method_name << " " << name;
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    const std::pair<std::string, int>& c = list[rng_(list.size())];
    DLOG(INFO) << "request to " << c.first << ":" << c.second;

    try {
      msgpack::rpc::client cli(c.first, c.second);
      cli.set_timeout(a_.timeout);
      return cli.call(method_name, name, a0, a1, a2).template get<R>();
    } catch (const std::exception& e) {
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw;
    }
  }

  //// async version
  template<typename R, typename Tuple>
  void random_async_vproxy(
      request_type req,
      const std::string& method_name,
      const Tuple& args) {
    std::vector<std::pair<std::string, int> > list;
    std::string name = args.template get<0>();
    get_members_(name, list);
    const std::pair<std::string, int>& c = list[rng_(list.size())];

    async_task_loop::template call_apply<R, Tuple>(
        c.first, c.second, method_name, args, a_, a_.timeout, req);
  }

  template<typename R>
  R broadcast_proxy0(
      const std::string& method_name,
      const std::string& name,
      pfi::lang::function<R(R, R)>& agg) {
    DLOG(INFO) << __func__ << " " << method_name << " " << name;
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

#ifndef NDEBUG
    for (size_t i = 0; i < list.size(); i++) {
      DLOG(INFO) << "request to " << list[i].first << ":" << list[i].second;
    }
#endif

    try {
      jubatus::server::common::mprpc::rpc_mclient c(list, a_.timeout,
                                            get_private_session_pool());
      return *(c.call(method_name, name, agg));
    } catch (const std::exception& e) {
      LOG(ERROR) << e.what();  // << " from " << c.first << ":" << c.second;
      throw;
    }
  }
  template<typename R, typename A>
  R broadcast_proxy1(
      const std::string& method_name,
      const std::string& name,
      const A& arg,
      pfi::lang::function<R(R, R)>& agg) {
    DLOG(INFO) << __func__ << " " << method_name << " " << name;
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

#ifndef NDEBUG
    for (size_t i = 0; i < list.size(); i++) {
      DLOG(INFO) << "request to " << list[i].first << ":" << list[i].second;
    }
#endif

    try {
      jubatus::server::common::mprpc::rpc_mclient c(list, a_.timeout,
                                            get_private_session_pool());
      return *(c.call(method_name, name, arg, agg));
    } catch (const std::exception& e) {
      LOG(ERROR) << e.what();  // << " from " << c.first << ":" << c.second;
      throw;
    }
  }

  // async task version
  template<typename R, typename Tuple>
  void broadcast_async_vproxy(
      request_type req,
      const std::string& method_name,
      const Tuple& args,
      pfi::lang::function<R(R, R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    std::string name = args.template get<0>();
    get_members_(name, list);

    async_task_loop::template call_apply<R, Tuple>(
        list, method_name, args, a_, a_.timeout, req, agg);
  }

  template<int N, typename R>
  R cht_proxy0(
      const std::string& method_name,
      const std::string& name,
      const std::string& id,
      pfi::lang::function<R(R, R)>& agg) {
    DLOG(INFO) << __func__ << " " << method_name << " " << name;
    std::vector<std::pair<std::string, int> > list;
    get_members_from_cht_(name, id, list, N);

#ifndef NDEBUG
    for (size_t i = 0; i < list.size(); i++) {
      DLOG(INFO) << "request to " << list[i].first << ":" << list[i].second;
    }
#endif

    try {
      jubatus::server::common::mprpc::rpc_mclient c(list, a_.timeout,
                                            get_private_session_pool());
      return *(c.call(method_name, name, id, agg));
    } catch (const std::exception& e) {
      LOG(ERROR) << N << " " << e.what();
      // << " from " << c.first << ":" << c.second;
      throw;
    }
  }
  template<int N, typename R, typename A0>
  R cht_proxy1(
      const std::string& method_name,
      const std::string& name,
      const std::string& id,
      const A0& arg,
      pfi::lang::function<R(R, R)>& agg) {
    DLOG(INFO) << __func__ << " " << method_name << " " << name;
    std::vector<std::pair<std::string, int> > list;
    get_members_from_cht_(name, id, list, N);

#ifndef NDEBUG
    for (size_t i = 0; i < list.size(); i++) {
      DLOG(INFO) << "request to " << list[i].first << ":" << list[i].second;
    }
#endif

    try {
      jubatus::server::common::mprpc::rpc_mclient c(list, a_.timeout,
                                            get_private_session_pool());
      return *(c.call(method_name, name, id, arg, agg));
    } catch (const std::exception& e) {
      LOG(ERROR) << e.what();  // << " from " << c.first << ":" << c.second;
      throw;
    }
  }
  template<int N, typename R, typename A0, typename A1>
  R cht_proxy2(
      const std::string& method_name,
      const std::string& name,
      const std::string& id,
      const A0& a0,
      const A1& a1,
      pfi::lang::function<R(R, R)>& agg) {
    DLOG(INFO) << __func__ << " " << method_name << " " << name;
    std::vector<std::pair<std::string, int> > list;
    get_members_from_cht_(name, id, list, N);

#ifndef NDEBUG
    for (size_t i = 0; i < list.size(); i++) {
      DLOG(INFO) << "request to " << list[i].first << ":" << list[i].second;
    }
#endif

    try {
      jubatus::server::common::mprpc::rpc_mclient c(list, a_.timeout,
                                            get_private_session_pool());
      return *(c.call(method_name, name, id, a0, a1, agg));
    } catch (const std::exception& e) {
      LOG(ERROR) << e.what();  // << " from " << c.first << ":" << c.second;
      throw;
    }
  }

  //// async version
  template<int N, typename R, typename Tuple>
  void cht_async_vproxy(
      request_type req,
      const std::string& method_name,
      const Tuple& args,
      pfi::lang::function<R(R, R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    std::string name = args.template get<0>();
    std::string id = args.template get<1>();
    get_members_from_cht_(name, id, list, N);

    async_task_loop::template call_apply<R, Tuple>(
        list, method_name, args, a_, a_.timeout, req, agg);
  }

  // get thread local session-pool
  msgpack::rpc::session_pool* get_private_session_pool() {
    extern __thread msgpack::rpc::session_pool* private_session_pool_;

    if (!private_session_pool_) {
      private_session_pool_ = new msgpack::rpc::session_pool();
      private_session_pool_->set_pool_time_limit(a_.session_pool_expire);
      private_session_pool_->set_pool_size_limit(a_.session_pool_size);
    }
    return private_session_pool_;
  }

  // async task
 public:
  class async_task_loop;

 public:
  template<typename Res>
  class async_task : public mp::enable_shared_from_this<async_task<Res> > {
   public:
    typedef pfi::lang::shared_ptr<Res> result_ptr;
    typedef pfi::lang::function<Res(Res, Res)> reducer_type;

   public:
    async_task(
        async_task_loop* at_loop,
        const host_list_type& hosts,
        const std::string& method_name,
        request_type req,
        reducer_type reducer = reducer_type())
        : at_loop_(at_loop),
          hosts_(hosts),
          method_name_(method_name),
          req_(req),
          reducer_(reducer),
          running_count_(0),
          cancelled_(false),
          timer_id_(-1) {
    }
    virtual ~async_task() {
      cancel_timeout();
    }

    void done_one(msgpack::rpc::future f, int future_index) {
      namespace jcm = jubatus::server::common::mprpc;

      mp::pthread_scoped_lock _l(lock_);

      if (!cancelled_) {
        try {
          done_one_inner(f, future_index);
        } catch (...) {
          // continue process next result when exception thrown.
          // store exception_thrower to list of errors

          errors_.push_back(
              jcm::rpc_error(hosts_[future_index].first,
                             hosts_[future_index].second,
                             jubatus::exception::get_current_exception()));
        }
      }

      futures_[future_index] = msgpack::rpc::future();

      --running_count_;
      if (!cancelled_ && running_count_ <= 0) {
        cancel_timeout();
        req_.result<Res>(aggregate_results());
      }
    }

    Res aggregate_results() {
      if (results_.size() == 0) {
        return Res();  // TODO(kmaehashi): we should raise exception ?
      }

      if (!reducer_ || results_.size() == 1) {
        return *(results_[0]);
      }

      Res tmp_result = *(results_[0]);
      for (size_t i = 1; i < results_.size(); ++i) {
        tmp_result = reducer_(tmp_result, *(results_[i]));
      }
      return tmp_result;
    }

    void set_timeout(int timeout_sec) {
      cancel_timeout();
      if (timeout_sec > 0) {
        msgpack::rpc::loop loop = at_loop_->pool().get_loop();
        timer_id_ = loop->add_timer(
            timeout_sec, 0,
            mp::bind(&async_task<Res>::on_timeout, this->shared_from_this()));
      }
    }

    void cancel_timeout() {
      if (timer_id_ >= 0) {
        msgpack::rpc::loop loop = at_loop_->pool().get_loop();
        loop->remove_timer(timer_id_);
        timer_id_ = -1;
      }
    }

    bool on_timeout() {
      mp::pthread_scoped_lock _l(lock_);
      if (!cancelled_) {
        cancelled_ = true;
        for (size_t i = 0; i < futures_.size(); ++i) {
          if (!futures_[i].is_finished()) {
            futures_[i].cancel();
          }
        }

        req_.error(msgpack::rpc::TIMEOUT_ERROR);
      }
      return true;
    }

    template<typename Args>
    void call_apply(
        const std::string& method_name,
        const Args& args,
        int timeout_sec) {
      mp::pthread_scoped_lock _l(lock_);

      running_count_ = hosts_.size();
      if (timeout_sec > 0) {
        set_timeout(timeout_sec);
      }

      msgpack::rpc::session_pool& pool = at_loop_->pool();
      for (size_t i = 0; i < hosts_.size(); ++i) {
        msgpack::rpc::session s = pool.get_session(hosts_[i].first,
                                                   hosts_[i].second);

        msgpack::rpc::future f = s.call_apply(method_name, args);
        futures_.push_back(f);
        f.attach_callback(
            mp::bind(&async_task<Res>::done_one, this->shared_from_this(),
                     mp::placeholders::_1, i));
      }
    }

   private:
    async_task_loop* at_loop_;

    host_list_type hosts_;
    std::string method_name_;
    request_type req_;
    reducer_type reducer_;

    int running_count_;
    bool cancelled_;
    int timer_id_;

    std::vector<msgpack::rpc::future> futures_;
    std::vector<result_ptr> results_;
    std::vector<jubatus::server::common::mprpc::rpc_error> errors_;

    mp::pthread_recursive_mutex lock_;

    void done_one_inner(msgpack::rpc::future f, int future_index) {
      namespace jcm = jubatus::server::common::mprpc;

      try {
        result_ptr result(new Res(f.get<Res>()));
        results_.push_back(result);
      }
      JUBATUS_MSGPACKRPC_EXCEPTION_DEFAULT_HANDLER(method_name_);
    }
  };

  // async task loop
 public:
  class async_task_loop : public mp::enable_shared_from_this<async_task_loop> {
   public:
    explicit async_task_loop(const keeper_argv& a) {
      pool_.set_pool_time_limit(a.session_pool_expire);
      pool_.set_pool_size_limit(a.session_pool_size);
    }

    void run() {
      while (true) {
        pool_.run_once();
      }
    }

    msgpack::rpc::session_pool& pool() {
      return pool_;
    }

    static async_task_loop* startup(const keeper_argv& a) {
      async_task_loop* at_loop = new async_task_loop(a);

#if 0
      pfi::concurrent::thread thr(pfi::lang::bind(&async_task_loop::run,
                                                  at_loop));
      thr.start();
#else
      at_loop->pool().start(2);
      // Use mpio's event loop instead of async_task_loop's one.
      // Note: mpio's event loop start() requires thread_num > 1. ( mpio bug? )
#endif

      return at_loop;
    }

    static async_task_loop* get_private_async_task_loop(const keeper_argv& a) {
      if (!private_async_task_loop_) {
        private_async_task_loop_ = startup(a);
      }

      return private_async_task_loop_;
    }

    template<typename Res, typename Args>
    static void call_apply(
        const host_list_type& hosts,
        const std::string& method_name,
        const Args& args,
        const keeper_argv& a,
        int timeout_sec,
        request_type req,
        typename async_task<Res>::reducer_type reducer =
        typename async_task<Res>::reducer_type()) {
      async_task_loop* at_loop = get_private_async_task_loop(a);
      mp::shared_ptr<async_task<Res> > task(
          new async_task<Res>(at_loop, hosts, method_name, req, reducer));
      task->template call_apply<Args>(method_name, args, timeout_sec);
    }

    template<typename Res, typename Args>
    static void call_apply(
        const std::string& host,
        int port,
        const std::string& method_name,
        const Args& args,
        const keeper_argv& a,
        int timeout_sec,
        request_type req,
        typename async_task<Res>::reducer_type reducer =
        typename async_task<Res>::reducer_type()) {
      host_list_type hosts;
      hosts.push_back(std::make_pair(host, port));
      call_apply<Res, Args>(hosts, method_name, args, a, timeout_sec, req,
                            reducer);
    }

   protected:
    msgpack::rpc::session_pool pool_;

    static __thread async_task_loop* private_async_task_loop_;
  };
};

}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_KEEPER_HPP_
