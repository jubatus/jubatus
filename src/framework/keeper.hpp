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
#include "../common/mprpc/rpc_client.hpp"
#include "../common/shared_ptr.hpp"

#include "server_util.hpp"
#include <glog/logging.h>
#include <iostream>

namespace jubatus {
namespace framework {

class keeper : public pfi::network::mprpc::rpc_server {
 public:
  keeper(const keeper_argv& a);
  virtual ~keeper();
  int run();

  template <typename R>
  void register_random(std::string method_name) {
    pfi::lang::function<R(std::string)> f = pfi::lang::bind(&keeper::template random_proxy<R>, this, method_name, pfi::lang::_1);
    add(method_name, f);
  }
  template <typename R, typename A0> //, typename A1, typename A2>
  void register_random(std::string method_name) {
    pfi::lang::function<R(std::string,A0)> f = pfi::lang::bind(&keeper::template random_proxy<R,A0>, this, method_name, pfi::lang::_1, pfi::lang::_2);
    add(method_name, f);
  }
  template <typename R, typename A0, typename A1>//, typename A2>
  void register_random(std::string method_name) {
    pfi::lang::function<R(std::string,A0,A1)> f = pfi::lang::bind(&keeper::template random_proxy<R,A0,A1>, this, method_name, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3);
    add(method_name,f); 
  }
  
  template <typename R, typename A0>
  void register_broadcast(std::string method_name,
                          pfi::lang::function<R(R,R)> agg){//pfi::lang::function<R(R,R)>& agg) {
    pfi::lang::function<R(std::string, A0)> f =
      pfi::lang::bind(&keeper::template broadcast_proxy<R, A0>, this, method_name, pfi::lang::_1, pfi::lang::_2,
                      agg);
    add(method_name, f);
  }
  template <typename R>
  void register_broadcast(std::string method_name,
                          pfi::lang::function<R(R,R)> agg){//pfi::lang::function<R(R,R)>& agg) {
    pfi::lang::function<R(std::string)> f =
      pfi::lang::bind(&keeper::template broadcast_proxy<R>, this, method_name, pfi::lang::_1, 
                      agg);
    add(method_name, f);
  }


  template <typename R>
  void register_cht(std::string method_name, pfi::lang::function<R(R,R)> agg) {
    pfi::lang::function<R(std::string, std::string)> f =
      pfi::lang::bind(&keeper::template cht_proxy<R>, this, method_name, pfi::lang::_1, pfi::lang::_2, agg);
    add(method_name, f);
  }
  template <typename R, typename A0>
  void register_cht(std::string method_name, pfi::lang::function<R(R,R)> agg) {
    pfi::lang::function<R(std::string, std::string, A0)> f =
      pfi::lang::bind(&keeper::template cht_proxy<R,A0>, this, method_name, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, agg);
    add(method_name, f);
  }
  template <typename R, typename A0, typename A1>
  void register_cht(std::string method_name, pfi::lang::function<R(R,R)> agg) {
    pfi::lang::function<R(std::string, std::string, A0, A1)> f =
      pfi::lang::bind(&keeper::template cht_proxy<R, A0, A1>, this, method_name, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3, pfi::lang::_4, agg);
    add(method_name, f);
  }
  
 private:
  template <typename R>
  R random_proxy(const std::string& method_name, const std::string& name){
    //    {DLOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    if(list.empty())throw std::runtime_error(method_name + ": no worker serving");
    const std::pair<std::string, int>& c = list[rng_(list.size())];

    try{
      pfi::network::mprpc::rpc_client cli(c.first, c.second, a_.timeout);
      return cli.call<R(std::string)>(method_name)(name);
    }catch(const std::exception& e){
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw e;
    }
  }
  template <typename R, typename A>
  R random_proxy(const std::string& method_name, const std::string& name, const A& arg){
    //    {DLOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;

    get_members_(name, list);

    if(list.empty())throw std::runtime_error(method_name + ": no worker serving");
    const std::pair<std::string, int>& c = list[rng_(list.size())];

    try{
      pfi::network::mprpc::rpc_client cli(c.first, c.second, a_.timeout);
      return cli.call<R(std::string,A)>(method_name)(name, arg);
    }catch(const std::exception& e){
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw e;
    }
  }
  template <typename R, typename A0, typename A1>
  R random_proxy(const std::string& method_name, const std::string& name, const A0& a0, const A1& a1){
    std::vector<std::pair<std::string, int> > list;

    get_members_(name, list);

    if(list.empty())throw std::runtime_error(method_name + ": no worker serving");
    const std::pair<std::string, int>& c = list[rng_(list.size())];

    try{
      pfi::network::mprpc::rpc_client cli(c.first, c.second, a_.timeout);
      return cli.call<R(std::string,A0,A1)>(method_name)(name, a0, a1);
    }catch(const std::exception& e){
      LOG(ERROR) << e.what() << " from " << c.first << ":" << c.second;
      throw e;
    }
  }

  template <typename R>
  R broadcast_proxy(const std::string& method_name, const std::string& name,
                    pfi::lang::function<R(R,R)>& agg) {
    //    {DLOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;

    get_members_(name, list);
    if(list.empty())throw std::runtime_error(method_name + ": no worker serving");

    try{
      jubatus::common::mprpc::rpc_mclient c(list, a_.timeout);
      c.call_async(method_name, name);
      return c.join_all<R>(agg);
    }catch(const std::exception& e){
      LOG(ERROR) << e.what(); // << " from " << c.first << ":" << c.second;
      throw e;
    }
  }
  template <typename R, typename A>
  R broadcast_proxy(const std::string& method_name, const std::string& name, const A& arg,
                    pfi::lang::function<R(R,R)>& agg) {
    //    {DLOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;

    get_members_(name, list);
    if(list.empty())throw std::runtime_error(method_name + ": no worker serving");

    try{
      jubatus::common::mprpc::rpc_mclient c(list, a_.timeout);
      c.call_async(method_name, name, arg);
      std::cout << __LINE__ << " name:" << name << " method:" << method_name << std::endl;
      return c.join_all<R>(agg);
    }catch(const std::runtime_error& e){
      std::cout << __LINE__ << e.what() << std::endl;
      // LOG(ERROR) << e.what(); // << " from " << c.first << ":" << c.second;
      throw e;
    }
  }


  template <typename R>
  R cht_proxy(const std::string& method_name, const std::string& name, const std::string& id,
              pfi::lang::function<R(R,R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    {
      pfi::concurrent::scoped_lock lk(mutex_);
      jubatus::common::cht ht(zk_, name);
      ht.find(id, list);
    }
    if(list.empty())throw std::runtime_error(method_name + ": no worker serving");

    try{
      jubatus::common::mprpc::rpc_mclient c(list, a_.timeout);
      c.call_async(method_name, name, id);
      return c.join_all<R>(agg);
    }catch(const std::exception& e){
      LOG(ERROR) << e.what(); // << " from " << c.first << ":" << c.second;
      throw e;
    }
  }
  template <typename R, typename A0>
  R cht_proxy(const std::string& method_name, const std::string& name, const std::string& id, const A0& arg,
              pfi::lang::function<R(R,R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    {
      pfi::concurrent::scoped_lock lk(mutex_);
      jubatus::common::cht ht(zk_, name);
      ht.find(id, list);
    }
    if(list.empty())throw std::runtime_error(method_name + ": no worker serving");

    try{
      jubatus::common::mprpc::rpc_mclient c(list, a_.timeout);
      c.call_async(method_name, name, id, arg);
      return c.join_all<R>(agg);
    }catch(const std::exception& e){
      LOG(ERROR) << e.what(); // << " from " << c.first << ":" << c.second;
      throw e;
    }
  }
  template <typename R, typename A0, typename A1>
  R cht_proxy(const std::string& method_name, const std::string& name, const std::string& id, const A0& a0, const A1& a1,
              pfi::lang::function<R(R,R)>& agg) {
    std::vector<std::pair<std::string, int> > list;
    {
      pfi::concurrent::scoped_lock lk(mutex_);
      jubatus::common::cht ht(zk_, name);
      ht.find(id, list);
    }
    if(list.empty())throw std::runtime_error(method_name + ": no worker serving");

    try{
      jubatus::common::mprpc::rpc_mclient c(list, a_.timeout);
      c.call_async(method_name, name, id, a0, a1);
      return c.join_all<R>(agg);
    }catch(const std::exception& e){
      LOG(ERROR) << e.what(); // << " from " << c.first << ":" << c.second;
      throw e;
    }
  }

  void get_members_(const std::string& name, std::vector<std::pair<std::string, int> >& ret);

  keeper_argv a_;
  pfi::math::random::mtrand rng_;
  pfi::concurrent::mutex mutex_;
  common::cshared_ptr<common::lock_service> zk_;

};

}
} //namespace jubatus
