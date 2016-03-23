// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_FRAMEWORK_PROXY_HPP_
#define JUBATUS_SERVER_FRAMEWORK_PROXY_HPP_

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <jubatus/msgpack/rpc/client.h>
#include <msgpack.hpp>
#include "jubatus/util/concurrent/thread.h"
#include "jubatus/util/data/optional.h"
#include "jubatus/util/lang/function.h"
#include "jubatus/util/lang/bind.h"

#include "proxy_common.hpp"
#include "server_util.hpp"
#include "../common/logger/logger.hpp"
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

class proxy
    : public proxy_common, jubatus::server::common::mprpc::rpc_server {
 public:
  typedef msgpack::rpc::request request_type;
  typedef std::vector<std::pair<std::string, int> > host_list_type;

 public:
  explicit proxy(const proxy_argv& a);
  virtual ~proxy();

  int run();

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
      jubatus::util::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string> packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template<typename R, typename A0>
  void register_async_broadcast(
      const std::string& method_name,
      jubatus::util::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, A0> packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template<typename R, typename A0, typename A1>
  void register_async_broadcast(
      const std::string& method_name,
      jubatus::util::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, A0, A1> packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template<typename R, typename A0, typename A1, typename A2>
  void register_async_broadcast(
      const std::string& method_name,
      jubatus::util::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, A0, A1, A2>
      packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  template<typename R, typename A0, typename A1, typename A2, typename A3>
  void register_async_broadcast(
      const std::string& method_name,
      jubatus::util::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, A0, A1, A2, A3>
      packed_args_type;
    register_async_vbroadcast_inner<R, packed_args_type>(method_name, agg);
  }

  // async cht method ( arity 0-4 )
  template<int N, typename R>
  void register_async_cht(
      const std::string& method_name,
      jubatus::util::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, std::string>
      packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template<int N, typename R, typename A0>
  void register_async_cht(
      const std::string& method_name,
      jubatus::util::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, std::string, A0>
      packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template<int N, typename R, typename A0, typename A1>
  void register_async_cht(
      const std::string& method_name,
      jubatus::util::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, std::string, A0, A1>
      packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template<int N, typename R, typename A0, typename A1, typename A2>
  void register_async_cht(
      const std::string& method_name,
      jubatus::util::lang::function<R(R, R)> agg) {
    typedef typename msgpack::type::tuple<std::string, std::string, A0, A1, A2>
      packed_args_type;
    register_async_vcht_inner<N, R, packed_args_type>(method_name, agg);
  }

  template<int N, typename R, typename A0, typename A1, typename A2,
           typename A3>
  void register_async_cht(
      const std::string& method_name,
      jubatus::util::lang::function<R(R, R)> agg) {
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
        &proxy::template random_async_vproxy<R, Tuple>,
        this, /* request */_1, method_name, /* packed_args */_2);
    add_async_vmethod<Tuple>(method_name, f);
  }

  template<typename R, typename Tuple>
  void register_async_vbroadcast_inner(
      const std::string& method_name,
      const jubatus::util::lang::function<R(R, R)>& agg) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    typedef typename common::mprpc::async_vmethod<Tuple>::type vfunc_type;

    vfunc_type f = mp::bind(
        &proxy::template broadcast_async_vproxy<R, Tuple>,
        this, /* request */_1, method_name, /* packed_args */_2, agg);
    add_async_vmethod<Tuple>(method_name, f);
  }

  template<int N, typename R, typename Tuple>
  void register_async_vcht_inner(
      const std::string& method_name,
      const jubatus::util::lang::function<R(R, R)>& agg) {
    using mp::placeholders::_1;
    using mp::placeholders::_2;
    typedef typename common::mprpc::async_vmethod<Tuple>::type vfunc_type;

    vfunc_type f = mp::bind(
        &proxy::template cht_async_vproxy<N, R, Tuple>,
        this, /* request */_1, method_name, /* packed_args */_2, agg);
    add_async_vmethod<Tuple>(method_name, f);
  }

 private:
  template<typename R, typename Tuple>
  void random_async_vproxy(
      request_type req,
      const std::string& method_name,
      const Tuple& args) {
    std::vector<std::pair<std::string, int> > list;
    std::string name = args.template get<0>();

    update_request_counter();

    get_members_(name, list);
    const std::pair<std::string, int>& c = list[rng_(list.size())];

    update_forward_counter();

    async_task_loop::template call_apply<R, Tuple>(
        c.first, c.second, method_name, args, a_, a_.interconnect_timeout, req);
  }

  template<typename R, typename Tuple>
  void broadcast_async_vproxy(
      request_type req,
      const std::string& method_name,
      const Tuple& args,
      jubatus::util::lang::function<R(R, R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    std::string name = args.template get<0>();

    update_request_counter();

    get_members_(name, list);

    update_forward_counter(list.size());

    async_task_loop::template call_apply<R, Tuple>(
        list, method_name, args, a_, a_.interconnect_timeout, req, agg);
  }

  template<int N, typename R, typename Tuple>
  void cht_async_vproxy(
      request_type req,
      const std::string& method_name,
      const Tuple& args,
      jubatus::util::lang::function<R(R, R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    std::string name = args.template get<0>();
    std::string id = args.template get<1>();

    update_request_counter();

    get_members_from_cht_(name, id, list, N);

    update_forward_counter(list.size());

    async_task_loop::template call_apply<R, Tuple>(
        list, method_name, args, a_, a_.interconnect_timeout, req, agg);
  }

 public:
  class async_task_loop;

 public:
  /*
   * async_task manages the context of proxy session.
   */
  template<typename Res>
  class async_task : public mp::enable_shared_from_this<async_task<Res> > {
   public:
    typedef jubatus::util::lang::shared_ptr<Res> result_ptr;
    typedef jubatus::util::lang::function<Res(Res, Res)> reducer_type;

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

    /*
     * Called each time when one response is received from remote server.
     */
    void done_one(msgpack::rpc::future f, int future_index) {
      namespace jcm = jubatus::server::common::mprpc;

      mp::pthread_scoped_lock _l(lock_);

      if (!cancelled_) {
        try {
          try {
            done_one_inner(f, future_index);
          } catch (const jubatus::server::common::mprpc::rpc_io_error&) {
            if (!transport_error_) {
              transport_error_ = "connect error in proxy";
            }
            throw;
          } catch (const jubatus::server::common::mprpc::rpc_timeout_error&) {
            if (!transport_error_) {
              transport_error_ = "timeout error in proxy";
            }
            throw;
          }
        } catch (...) {
          // continue process next result when exception thrown.
          // store exception_thrower to list of errors

          errors_.push_back(
              jcm::rpc_error(hosts_[future_index].first,
                             hosts_[future_index].second,
                             core::common::exception::get_current_exception()));
        }
      }

      futures_[future_index] = msgpack::rpc::future();

      --running_count_;
      if (!cancelled_ && running_count_ <= 0) {
        cancel_timeout();
        if (errors_.size() > 0) {
          LOG(WARNING) << "error occurred in " << errors_.size()
                       << " out of " << futures_.size() << " requests";
          LOG(WARNING) << jubatus::server::common::mprpc::to_string(
              jubatus::server::common::mprpc::error_multi_rpc(errors_));

          if (transport_error_) {
            req_.error(*transport_error_);
          } else {
            req_.error(get_error_message(errors_[0]));
          }
        } else {
          req_.result<Res>(aggregate_results());
        }
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
            // cancel the request
            futures_[i].cancel();

            // cancelled sessions (e.g., connections) cannot be reused,
            // so remove them from the session pool.
            at_loop_->pool().remove_session(sessions_[i]);

            LOG(WARNING) << "request timeout occurred: " << hosts_[i].first
                         << ":" << hosts_[i].second;
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
      // enable proxy::async_task timeout management
      if (timeout_sec > 0) {
        set_timeout(timeout_sec);
      }

      msgpack::rpc::session_pool& pool = at_loop_->pool();
      for (size_t i = 0; i < hosts_.size(); ++i) {
        msgpack::rpc::session s = pool.get_session(hosts_[i].first,
                                                   hosts_[i].second);
        // disable msgpack::rpc::session's timeout.
        // because session timeout is managed by proxy::async_task
        s.set_timeout(0);

        // apply async method call and set its callback
        msgpack::rpc::future f = s.call_apply(method_name, args);
        futures_.push_back(f);
        sessions_.push_back(s);
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
    std::vector<msgpack::rpc::session> sessions_;
    std::vector<result_ptr> results_;
    std::vector<jubatus::server::common::mprpc::rpc_error> errors_;
    jubatus::util::data::optional<std::string> transport_error_;

    mp::pthread_recursive_mutex lock_;

    void done_one_inner(msgpack::rpc::future f, int future_index) {
      namespace jcm = jubatus::server::common::mprpc;

      try {
        result_ptr result(new Res(f.get<Res>()));
        results_.push_back(result);
      }
      JUBATUS_MSGPACKRPC_EXCEPTION_DEFAULT_HANDLER(method_name_);
    }
  };  // class async_task

 private:
  static std::string get_error_message(
      const jubatus::server::common::mprpc::rpc_error& err);

 public:
  class async_task_loop : public mp::enable_shared_from_this<async_task_loop> {
   public:
    explicit async_task_loop(const proxy_argv& a) {
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

    /*
     * Returns async_task_loop for this thread (note that async_task_loop
     * is in the thread local storage).
     * If async_task_loop is not yet created for this thread, starts a new
     * one and returns it.
     */
    static async_task_loop* get_private_async_task_loop(const proxy_argv& a) {
      if (!private_async_task_loop_) {
        private_async_task_loop_ = startup(a);
      }

      return private_async_task_loop_;
    }

    /*
     * call_apply (for multiple servers)
     */
    template<typename Res, typename Args>
    static void call_apply(
        const host_list_type& hosts,
        const std::string& method_name,
        const Args& args,
        const proxy_argv& a,
        int timeout_sec,
        request_type req,
        typename async_task<Res>::reducer_type reducer =
        typename async_task<Res>::reducer_type()) {
      async_task_loop* at_loop = get_private_async_task_loop(a);
      mp::shared_ptr<async_task<Res> > task(
          new async_task<Res>(at_loop, hosts, method_name, req, reducer));
      task->template call_apply<Args>(method_name, args, timeout_sec);
    }

    /*
     * call_apply (for single server)
     */
    template<typename Res, typename Args>
    static void call_apply(
        const std::string& host,
        int port,
        const std::string& method_name,
        const Args& args,
        const proxy_argv& a,
        int timeout_sec,
        request_type req,
        typename async_task<Res>::reducer_type reducer =
        typename async_task<Res>::reducer_type()) {
      host_list_type hosts;
      hosts.push_back(std::make_pair(host, port));
      call_apply<Res, Args>(hosts, method_name, args, a, timeout_sec, req,
                            reducer);
    }

   private:
    static async_task_loop* startup(const proxy_argv& a) {
      async_task_loop* at_loop = new async_task_loop(a);

#if 0
      jubatus::util::concurrent::thread thr(
          jubatus::util::lang::bind(&async_task_loop::run, at_loop));
      thr.start();
#else
      at_loop->pool().start(2);
      // Use mpio's event loop instead of async_task_loop's one.
      // Note: mpio's event loop start() requires thread_num > 1. ( mpio bug? )
#endif

      return at_loop;
    }

   protected:
    msgpack::rpc::session_pool pool_;

    static __thread async_task_loop* private_async_task_loop_;
  };  // class async_task_loop
};  // class proxy

}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_PROXY_HPP_
