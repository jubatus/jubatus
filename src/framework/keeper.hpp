// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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
#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>
#include <pficommon/math/random.h>

#include "../common/lock_service.hpp"
#include "../common/cht.hpp"

#include "server_util.hpp"

namespace jubatus {
namespace framework {

class keeper : public pfi::network::mprpc::rpc_server {
 public:
  keeper(const keeper_argv& a);
  virtual ~keeper();
  int run();

  template <typename Q>
  void register_random_update(std::string method_name) {
    pfi::lang::function<int(std::string, Q)> f =
      pfi::lang::bind(&keeper::template random_proxy<int, Q>, this, method_name, pfi::lang::_1, pfi::lang::_2);
    add(method_name, f);
  }

  template <typename R, typename Q>
  void register_random_analysis(std::string method_name) {
    pfi::lang::function<R(std::string, Q)> f =
      pfi::lang::bind(&keeper::template random_proxy<R, Q>, this, method_name, pfi::lang::_1, pfi::lang::_2);
    add(method_name, f);
  }

  template <typename Q>
  void register_broadcast_update(std::string method_name) {
    pfi::lang::function<int(std::string, Q)> f =
      pfi::lang::bind(&keeper::template broadcast_proxy<int,Q>, this, method_name, pfi::lang::_1, pfi::lang::_2);
    add(method_name, f);
  }

  template <typename R, typename Q>
  void register_broadcast_analysis(std::string method_name) {
    pfi::lang::function<R(std::string, Q)> f =
      pfi::lang::bind(&keeper::template broadcast_proxy<R, Q>, this, method_name, pfi::lang::_1, pfi::lang::_2);
    add(method_name, f);
  }

  template <typename Q>
  void register_cht_update(std::string method_name) {
    pfi::lang::function<int(std::string, std::string, Q)> f =
      pfi::lang::bind(&keeper::template cht_proxy<int, Q>, this, method_name, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3);
    add(method_name, f);
  }
  template <typename R, typename Q>
  void register_cht_analysis(std::string method_name) {
    pfi::lang::function<R(std::string, std::string, Q)> f =
      pfi::lang::bind(&keeper::template cht_proxy<R, Q>, this, method_name, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3);
    add(method_name, f);
  }
  
 private:
  template <typename R, typename A>
  R random_proxy(const std::string& method_name, const std::string& name, const A& arg) {
    //    {DLOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;

    get_members_(name, list);

    const std::pair<std::string, int>& c = list[rng_(list.size())];
    if(list.empty())throw std::runtime_error(method_name + ": no worker serving");

    // this code didn't work: rpc_client instance cannot be desctucted so too much live connections generated
    // and accept/read thread in pficommon server exhausted.
    //return pfi::network::mprpc::rpc_client(c.first, c.second, a_.timeout).call<R(std::string,A)>(method_name)(name, arg);

    try{
      pfi::network::mprpc::rpc_client cli(c.first, c.second, a_.timeout);
      return cli.call<R(std::string,A)>(method_name)(name, arg);
    }catch(const std::exception& e){
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw e;
    }
  }

  template <typename R, typename A>
  // FIXME: modify return type
  R broadcast_proxy(const std::string& method_name, const std::string& name, const A& arg) {
    //    {LOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;

    get_members_(name, list);
    //    std::vector<R> results;
    // FIXME: needs global lock here
    if(list.empty())throw std::runtime_error(method_name + ": no worker serving");

    pfi::network::mprpc::rpc_client cli(list[0].first, list[0].second, a_.timeout);
    R res = cli.call<R(std::string,A)>(method_name)(name,arg);

    for (size_t i = 1; i < list.size(); ++i) {
      const std::pair<std::string, int>& c = list[i];
      pfi::network::mprpc::rpc_client cli(c.first, c.second, a_.timeout);
      try{
	res = cli.call<R(std::string,A)>(method_name)(name,arg);
      }catch(const std::exception& e){
	LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
	continue;
      }
      //      results.push_back(res);
    }
    return res;
  }

  template <typename R, typename A>
  R cht_proxy(const std::string& method_name, const std::string& name, const std::string& key, const A& arg) {

    std::vector<std::pair<std::string, int> > list;
    {
      pfi::concurrent::scoped_lock lk(mutex_);
      jubatus::common::cht ht(zk_, name);
      ht.find(key, list);
    }
    if(list.empty())throw std::runtime_error(method_name + ": no worker serving");

    pfi::network::mprpc::rpc_client cli(list[0].first, list[0].second, a_.timeout);
    R result = cli.call<R(std::string,A)>(method_name)(name,arg);

    for(size_t i=1; i<list.size(); ++i){
      const std::pair<std::string, int>& c = list[i];
      pfi::network::mprpc::rpc_client cli(c.first, c.second, a_.timeout);
      try{
	result = cli.call<R(std::string,std::string,A)>(method_name)(name, key, arg);
      }catch(const std::exception& e){
	LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
	continue;
      }
    }
    return result;
  }
  void get_members_(const std::string& name, std::vector<std::pair<std::string, int> >& ret);

  keeper_argv a_;
  pfi::math::random::mtrand rng_;
  pfi::concurrent::mutex mutex_;
  pfi::lang::shared_ptr<common::lock_service> zk_;

};

}
} //namespace jubatus
