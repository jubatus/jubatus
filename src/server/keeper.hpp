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
#include <pficommon/lang/function.h>
#include <pficommon/lang/bind.h>
#include <pficommon/network/mprpc.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>
#include <pficommon/math/random.h>

#include "../common/zk.hpp"
#include "../common/membership.hpp"

#include "server_util.hpp"

static const std::string ACTOR_BASE_PATH = "";

namespace jubatus {

class keeper : public pfi::network::mprpc::rpc_server {
 public:
  keeper(const jubatus::keeper_argv& a)
    : pfi::network::mprpc::rpc_server(a.timeout),
      a_(a),
      zk_(a.z, a.timeout)
  {
    register_broadcast_analysis<int, std::string>("save");
    register_broadcast_update<std::string>("load");
  }

  int start(){
    return this->serv(a_.port, a_.threadnum);
  };

  template <typename Q>
  void register_random_update(std::string name) {
    pfi::lang::function<int(std::string, Q)> f =
      pfi::lang::bind(&keeper::template random_proxy<int, Q>, this, pfi::lang::_1, pfi::lang::_2);
    add(name, f);
  }

  template <typename R, typename Q>
  void register_random_analysis(std::string name) {
    pfi::lang::function<R(std::string, Q)> f =
      pfi::lang::bind(&keeper::template random_proxy<R, Q>, this, pfi::lang::_1, pfi::lang::_2);
    add(name, f);
  }

  template <typename Q>
  void register_broadcast_update(std::string name) {
    pfi::lang::function<std::vector<int>(std::string, Q)> f =
      pfi::lang::bind(&keeper::template broadcast_proxy<int,Q>, this, pfi::lang::_1, pfi::lang::_2);
    add(name, f);
  }

  template <typename R, typename Q>
  void register_broadcast_analysis(std::string name) {
    pfi::lang::function<std::vector<R>(std::string, Q)> f =
        pfi::lang::bind(&keeper::template broadcast_proxy<R, Q>, this, name, pfi::lang::_1);
    add(name, f);
  }

  template <typename R, typename Q>
  void register_cht(std::string name) {
    pfi::lang::function<std::vector<R>(std::string, std::string, Q)> f =
      pfi::lang::bind(&keeper::template broadcast_proxy<R, Q>, this, pfi::lang::_1, pfi::lang::_2, pfi::lang::_3);
    add(name, f);
  }
  
 private:
  template <typename R, typename A>
  R random_proxy(const std::string& name, const A& arg) {
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);
    const std::pair<std::string, int>& c = list[rng_(list.size())];

    return pfi::network::mprpc::rpc_client(c.first, c.second, a_.timeout).call<R(A)>(name)(arg);
  }

  template <typename R, typename A>
  std::vector<R> broadcast_proxy(const std::string& name, const A& arg) {
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    std::vector<R> results;
    for (size_t i = 0; i < list.size(); ++i) {
      const std::pair<std::string, int>& c = list[i];
      R res = pfi::network::mprpc::rpc_client(c.first, c.second, a_.timeout).call<R(A)>(name)(arg);
      results.push_back(res);
    }
    return results;
  }

  template <typename R, typename A> //FIXME
  std::vector<R> cht_proxy(const std::string& name, const std::string& key, const A& arg) {
    std::vector<std::pair<std::string, int> > list;
    get_members_(name, list);

    std::vector<R> results;
    for (size_t i = 0; i < list.size(); ++i) {
      const std::pair<std::string, int>& c = list[i];
      R res = pfi::network::mprpc::rpc_client(c.first, c.second, a_.timeout).call<R(A)>(name)(arg);
      results.push_back(res);
    }
    return results;
  }

  void get_members_(const std::string& name, std::vector<std::pair<std::string, int> >& ret){
    using namespace std;
    ret.clear();
    vector<string> list;
    string path = ACTOR_BASE_PATH + "/" + name + "/nodes";

    {
      pfi::concurrent::scoped_lock lk(mutex_);
      zk_.list(path, list);
    }
    vector<string>::const_iterator it;

    // FIXME:
    // do you return all server list? it can be very large
    for(it = list.begin(); it!= list.end(); ++it){
      string ip;
      int port;
      jubatus::revert(*it, ip, port);
      ret.push_back(make_pair(ip,port));
    }
  }

  jubatus::keeper_argv a_;
  pfi::math::random::mtrand rng_;
  pfi::concurrent::mutex mutex_;
  zk zk_;
};

}
