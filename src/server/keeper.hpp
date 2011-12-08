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
#include "../common/membership.hpp"

#include "server_util.hpp"

namespace jubatus {
namespace server {

class keeper : public pfi::network::mprpc::rpc_server {
 public:
  keeper(const jubatus::keeper_argv& a);
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

    pfi::network::mprpc::rpc_client cli(c.first, c.second, a_.timeout);
    //    {DLOG(INFO)<< "accssing to " << c.first << " " << c.second;}
    return cli.call<R(std::string,A)>(method_name)(name, arg);
  }

  template <typename R, typename A>
  // FIXME: modify return type
  R broadcast_proxy(const std::string& method_name, const std::string& name, const A& arg) {
    //    {LOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);
    //    std::vector<R> results;
    // FIXME: needs global lock here
    R res;
    for (size_t i = 0; i < list.size(); ++i) {
      const std::pair<std::string, int>& c = list[i];
      pfi::network::mprpc::rpc_client cli(c.first, c.second, a_.timeout);
      res = cli.call<R(std::string,A)>(method_name)(name,arg);
      //      results.push_back(res);
    }
    {LOG(INFO)<< __func__;}
    //    return results;
    return res;
  }

  template <typename R, typename A>
  R cht_proxy(const std::string& method_name, const std::string& name, const std::string& key, const A& arg) {
    {LOG(INFO)<< __func__ << " " << method_name << " " << name;}
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    // FIXME: resolve name here

    const std::pair<std::string, int>& c = list[0];
    pfi::network::mprpc::rpc_client cli(c.first, c.second, a_.timeout);
    R result = cli.call<R(std::string,std::string,A)>(method_name)(name, key, arg);
    return result;
  }
  void get_members_(const std::string& name, std::vector<std::pair<std::string, int> >& ret);

  jubatus::keeper_argv a_;
  pfi::math::random::mtrand rng_;
  pfi::concurrent::mutex mutex_;
  pfi::lang::shared_ptr<common::lock_service> zk_;
};

}
} //namespace jubatus
