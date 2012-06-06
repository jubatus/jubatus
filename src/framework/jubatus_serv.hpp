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

#include <pficommon/network/mprpc.h>

#include "server_util.hpp"
#include "mixer.hpp"
#include "mixable.hpp"

#include "../common/shared_ptr.hpp"
#include "../common/global_id_generator.hpp"

using namespace pfi::concurrent;

namespace jubatus { namespace framework {

class jubatus_serv : pfi::lang::noncopyable {

public:
  jubatus_serv(const server_argv& a, const std::string& base_path = "/tmp");
  virtual ~jubatus_serv(){};

  virtual int start(pfi::network::mprpc::rpc_server& serv);
  void register_mixable(mixable0* m);

  pfi::concurrent::rw_mutex& get_rw_mutex(){ return m_; };
  void use_cht();

  std::map<std::string, std::map<std::string,std::string> > get_status() const;
  std::string get_server_identifier()const;

#ifdef HAVE_ZOOKEEPER_H
  void join_to_cluster(common::cshared_ptr<jubatus::common::lock_service>);
  
  std::string get_storage(int i);

  std::vector<std::string> get_diff_impl(int);
  int put_diff_impl(std::vector<std::string> unpacked);
  void do_mix(const std::vector<std::pair<std::string,int> >& v);
#endif

public:
  void updated();

  bool save(std::string id);
  bool load(std::string id);

  // after load( model_ was loaded from file ) called, users reset their own data
  // I'm afraid this function is useless
  virtual void after_load() =0;

  std::string get_ipaddr()const{ return a_.eth; };
  int get_port()const{ return a_.port; };
  int get_threadum()const{ return a_.threadnum; };
  void get_members(std::vector<std::pair<std::string,int> >&);

  std::vector<std::string> mix_agg(const std::vector<std::string>& lhs,
   				   const std::vector<std::string>& rhs);
    
protected:
  void build_local_path_(std::string& out, const std::string& type, const std::string& id){
    out = base_path_ + "/";
    out += type;
    out += "_";
    out += id;
    out += ".jc";
  };

  server_argv a_;
  unsigned int update_count_;

  std::vector<mixable0*> mixables_;
  common::cshared_ptr<mixer> mixer_;

#ifdef HAVE_ZOOKEEPER_H
  common::cshared_ptr<jubatus::common::lock_service> zk_;
  bool use_cht_;
#endif

  common::global_id_generator idgen_;

  pfi::concurrent::rw_mutex m_;
  const std::string base_path_;

};

}}

#define JRLOCK__(p) \
  pfi::concurrent::scoped_lock lk(rlock((p)->get_rw_mutex()));

#define JWLOCK__(p) \
  pfi::concurrent::scoped_lock lk(wlock((p)->get_rw_mutex())); \
  (p)->updated();
