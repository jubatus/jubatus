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

#include "graph_serv.hpp"

#include <cassert>
#include <sstream>
#include <pficommon/lang/cast.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/system/time_util.h>

#include "../common/util.hpp"
#include "../common/membership.hpp"
#include "../common/jsonconfig.hpp"
#include "../framework/aggregators.hpp"
#include "../framework/mixer/mixer_factory.hpp"
#include "../graph/graph_factory.hpp"
#include "graph_client.hpp"
#include "graph_types.hpp"

using pfi::system::time::clock_time;
using pfi::system::time::get_clock_time;
using namespace jubatus::common;
using namespace jubatus::framework;

namespace jubatus {
namespace server {

enum graph_serv_error {
  NODE_ALREADY_EXISTS = 0xDEADBEEF
};

namespace {

struct graph_serv_config {
  std::string method;
  jsonconfig::config parameter;

  template <typename Ar>
  void serialize(Ar& ar) {
    ar
        & MEMBER(method)
        & MEMBER(parameter);
  }
};

inline node_id uint642nodeid(uint64_t i) {
  return pfi::lang::lexical_cast<node_id, uint64_t>(i);
}

inline uint64_t nodeid2uint64(const node_id& id) {
  return pfi::lang::lexical_cast<uint64_t, node_id>(id);
}

inline node_id i2n(uint64_t i) {
  return uint642nodeid(i);
}

inline uint64_t n2i(const node_id& id) {
  return nodeid2uint64(id);
}
} // namespace

graph_serv::graph_serv(const framework::server_argv& a,
                       const cshared_ptr<lock_service>& zk)
    : server_base(a),
      idgen_(a.is_standalone()) {

#ifdef HAVE_ZOOKEEPER_H
  zk_ = zk;

  std::string counter_path;
  build_actor_path(counter_path, a.type, a.name);
  idgen_.set_ls(zk_, counter_path);
#endif

  mixer_.reset(mixer::create_mixer(a, zk));
  mixable_holder_.reset(new mixable_holder());

  mixer_->set_mixable_holder(mixable_holder_);
  mixable_holder_->register_mixable(&g_);
}

graph_serv::~graph_serv() {}

bool graph_serv::set_config(const std::string& config) {
  LOG(INFO) << __func__;

  jsonconfig::config conf_root(pfi::lang::lexical_cast<pfi::text::json::json>(config));
  graph_serv_config conf = jsonconfig::config_cast_check<graph_serv_config>(conf_root);

  config_ = config;
  g_.set_model(cshared_ptr<jubatus::graph::graph_base>(jubatus::graph::create_graph(conf.method, conf.parameter)));
  return true;
}

std::string graph_serv::get_config() const {
  check_set_config();
  return config_;
}

void graph_serv::check_set_config() const {
  if (!g_.get_model()) {
    throw JUBATUS_EXCEPTION(config_not_set());
  }
}

void graph_serv::get_status(status_t& status) const {
  check_set_config();

  status_t my_status;
  g_.get_model()->get_status(my_status);
  status.insert(my_status.begin(), my_status.end());
}

std::string graph_serv::create_node() { /* no lock here */
  check_set_config();

  uint64_t nid = idgen_.generate();
  std::string nid_str = pfi::lang::lexical_cast<std::string>(nid);

  if (!argv().is_standalone()) {
    // we dont need global locking, because getting unique id from zk
    // guarantees there'll be no data confliction
    {
      std::vector<std::pair<std::string, int> > nodes;
      find_from_cht_(nid_str, 2, nodes);
      if (nodes.empty()) {
        throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("no server found in cht: " + argv().name));
      }
      selective_create_node_(nodes[0], nid_str);

      for (size_t i = 1; i < nodes.size(); ++i) {
        try {
          selective_create_node_(nodes[i], nid_str);
        } catch(const graph::local_node_exists& e) { // pass through
        } catch(const graph::global_node_exists& e) {// pass through
        } catch(const std::runtime_error& e) { // error !
          LOG(WARNING) << "cannot crate " << i << "th replica: " << nodes[i].first << ":" << nodes[i].second;
          LOG(WARNING) << e.what();
        }
      }
    }
  } else {
    pfi::concurrent::scoped_wlock write_lk(rw_mutex());
    this->create_node_here(nid_str);
  }
  DLOG(INFO) << "node created: " << nid_str;
  return nid_str;
}

bool graph_serv::update_node(const std::string& id, const property& p) {
  check_set_config();

  g_.get_model()->update_node(n2i(id), p);
  DLOG(INFO) << "node updated: " << id;
  return true;
}

bool graph_serv::remove_node(const std::string& nid) {
  check_set_config();

  g_.get_model()->remove_node(n2i(nid));
  g_.get_model()->remove_global_node(n2i(nid));

  if (!argv().is_standalone()) {
    // send true remove_node_ to other machine,
    // if conflicts with create_node, users should re-run to ensure removal
    std::vector<std::pair<std::string, int> > members;
    get_members_(members);

    if (!members.empty()) {
      common::mprpc::rpc_mclient c(members, argv().timeout); //create global node

#ifndef NDEBUG
      for(size_t i =0; i < members.size(); i++) {
        DLOG(INFO) << "request to " << members[i].first << ":" << members[i].second;
      }
#endif

      try {
        c.call("remove_global_node", argv().name, nid, pfi::lang::function<int(int,int)>(&jubatus::framework::add<int>));
      } catch(const common::mprpc::rpc_no_result& e) { // pass through
        DLOG(INFO) << e.diagnostic_information(true);
      }
    }
  }
  DLOG(INFO) << "node removed: " << nid;
  return true;
}

//@cht
edge_id_t graph_serv::create_edge(const std::string& id, const edge_info& ei) {  /* no lock here */
  check_set_config();

  edge_id_t eid = idgen_.generate();
  //TODO: assert id==ei.src
  
  if (!argv().is_standalone()) {
    // we dont need global locking, because getting unique id from zk
    // guarantees there'll be no data confliction
    std::vector<std::pair<std::string, int> > nodes;
    find_from_cht_(ei.src, 2, nodes);
    if (nodes.empty()) {
      throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("no server found in cht: " + argv().name));
    }
    // TODO: assertion: nodes[0] should be myself
    {
      pfi::concurrent::scoped_wlock wirte_lk(rw_mutex());
      this->create_edge_here(eid, ei);
    }
    for (size_t i = 1; i < nodes.size(); ++i) {
      try {
	if (nodes[i].first == argv().eth && nodes[i].second == argv().port) {
	} else {
	  client::graph c(nodes[i].first, nodes[i].second, 5.0);
      DLOG(INFO) << "request to " << nodes[i].first << ":" << nodes[i].second;
	  c.create_edge_here(argv().name, eid, ei);
	}
      } catch(const graph::local_node_exists& e) { // pass through
      } catch(const graph::global_node_exists& e) {// pass through
      } catch(const std::runtime_error& e) { // error !
        LOG(WARNING) << "cannot create " << i << "th replica: " << nodes[i].first << ":" << nodes[i].second;
        LOG(WARNING) << e.what();
      }
    }
  } else {
    pfi::concurrent::scoped_wlock write_lk(rw_mutex());
    this->create_edge_here(eid, ei);
  }

  DLOG(INFO) << "edge created: (" << eid << ") " << ei.src << " => " << ei.tgt;
  return eid;
}

//@random
bool graph_serv::update_edge(const std::string&, edge_id_t eid, const edge_info& ei) {
  check_set_config();

  g_.get_model()->update_edge(eid, ei.p);
  DLOG(INFO) << "edge updated: (" << eid << ") " << ei.src << " => " << ei.tgt;
  return true;
}

bool graph_serv::remove_edge(const std::string&, const edge_id_t& id) {
  check_set_config();

  g_.get_model()->remove_edge(id);
  DLOG(INFO) << "edge removed: " << id;
  return true;
}

//@random
double graph_serv::get_centrality(const std::string& id, const centrality_type& s,
                                  const preset_query& q) const {
  check_set_config();

  if (s == 0) {
    jubatus::graph::preset_query q0;
    return g_.get_model()->centrality(n2i(id),
				      jubatus::graph::EIGENSCORE,
				      q0);
  } else {
    std::stringstream msg;
    msg << "unknown centrality type: " << s;
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(msg.str()));
  }
}

//@random
std::vector<node_id> graph_serv::get_shortest_path(const shortest_path_req& req) const {
  std::vector<jubatus::graph::node_id_t> ret0;
  jubatus::graph::preset_query q;
  framework::convert(req.q, q);
  g_.get_model()->shortest_path(n2i(req.src), n2i(req.tgt), req.max_hop, ret0, q);
  std::vector<node_id> ret;
  for (size_t i = 0; i < ret0.size(); ++i) {
    ret.push_back(i2n(ret0[i]));
  }
  return ret;
}

//update, broadcast
bool graph_serv::add_centrality_query(const preset_query& q0) {
  check_set_config();

  jubatus::graph::preset_query q;
  framework::convert<jubatus::preset_query, jubatus::graph::preset_query>(q0, q);
  g_.get_model()->add_centrality_query(q);
  DLOG(INFO) << "centrality query added";
  return true;
}

//update, broadcast
bool graph_serv::add_shortest_path_query(const preset_query& q0) {
  check_set_config();

  jubatus::graph::preset_query q;
  framework::convert<jubatus::preset_query, jubatus::graph::preset_query>(q0, q);
  g_.get_model()->add_shortest_path_query(q);
  DLOG(INFO) << "shortest path added";
  return true;
}

//update, broadcast
bool graph_serv::remove_centrality_query(const preset_query& q0) {
  check_set_config();

  jubatus::graph::preset_query q;
  framework::convert<jubatus::preset_query, jubatus::graph::preset_query>(q0, q);
  g_.get_model()->remove_centrality_query(q);
  DLOG(INFO) << "centrality query removed";
  return true;
}

//update, broadcast
bool graph_serv::remove_shortest_path_query(const preset_query& q0) {
  check_set_config();

  jubatus::graph::preset_query q;
  framework::convert<jubatus::preset_query, jubatus::graph::preset_query>(q0, q);
  g_.get_model()->remove_shortest_path_query(q);
  DLOG(INFO) << "shortest path removed";
  return true;
}

node_info graph_serv::get_node(const std::string& nid) const {
  check_set_config();

  jubatus::graph::node_info info;
  g_.get_model()->get_node(n2i(nid), info);
  jubatus::node_info ret;
  framework::convert<graph::node_info, jubatus::node_info>(info, ret);
  return ret;
}
//@random
edge_info graph_serv::get_edge(const std::string& nid, const edge_id_t& id) const {
  check_set_config();

  jubatus::graph::edge_info info;
  g_.get_model()->get_edge((jubatus::graph::edge_id_t)id, info);
  jubatus::edge_info ret;
  ret.p = info.p;
  ret.src = i2n(info.src);
  ret.tgt = i2n(info.tgt);
  return ret;
}

//@broadcast
bool graph_serv::update_index() {
  if (!argv().is_standalone()) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("manual mix is available only in standalone mode."));
  }
  check_set_config();

  clock_time start = get_clock_time();
  g_.get_model()->update_index();
  std::string diff;
  g_.get_model()->get_diff(diff);
  g_.get_model()->set_mixed_and_clear_diff(diff);
  clock_time end = get_clock_time();
  LOG(INFO) << "mix done manually and locally in " << (double)(end - start) << " secs.";
  return true;
}

bool graph_serv::clear() {
  check_set_config();

  if (g_.get_model()) {
    g_.get_model()->clear();
  }
  LOG(INFO) << "model cleared: " << argv().name;
  return true;
}

bool graph_serv::create_node_here(const std::string& nid) {
  try {
    graph::node_id_t id = pfi::lang::lexical_cast<graph::node_id_t>(nid);
    g_.get_model()->create_node(id);
    g_.get_model()->create_global_node(id);
  } catch(const graph::local_node_exists& e) { //pass through
  } catch(const graph::global_node_exists& e) {//pass through
  } catch(const std::runtime_error& e) {
    throw;
  }
  return true;
}

bool graph_serv::remove_global_node(const std::string& nid) {
  try {
    g_.get_model()->remove_global_node(n2i(nid));
  } catch(const graph::local_node_exists& e) {
  } catch(const graph::global_node_exists& e) {
  } catch(const std::runtime_error& e) {
    throw;
  }
  DLOG(INFO) << "global node removed";
  return true;
} //update internal

bool graph_serv::create_edge_here(edge_id_t eid, const edge_info& ei) {
  try {
    g_.get_model()->create_edge(eid, n2i(ei.src), n2i(ei.tgt));
    g_.get_model()->update_edge(eid, ei.p);
  } catch(const graph::graph_exception& e) {
    throw;
  }
  return true;
}

void graph_serv::selective_create_node_(const std::pair<std::string,int>& target,
                                        const std::string nid_str) {
  if (target.first == argv().eth && target.second == argv().port) {
    pfi::concurrent::scoped_wlock write_lk(rw_mutex());
    this->create_node_here(nid_str);
  } else {
    // must not lock here
    client::graph c(target.first, target.second, 5.0);
    c.create_node_here(argv().name, nid_str);
  }
}

void graph_serv::find_from_cht_(const std::string& key,
                                size_t n,
                                std::vector<std::pair<std::string, int> >& out) {
  out.clear();
#ifdef HAVE_ZOOKEEPER_H
  common::cht ht(zk_, argv().type, argv().name);
  ht.find(key, out, n); //replication number of local_node
#else
  //cannot reach here, assertion!
  assert(argv().is_standalone());
  //out.push_back(make_pair(argv().eth, argv().port));
#endif
}

void graph_serv::get_members_(std::vector<std::pair<std::string, int> >& ret) {
  ret.clear();
#ifdef HAVE_ZOOKEEPER_H
  common::get_all_actors(*zk_, argv().type, argv().name, ret);

  if (ret.empty()) {
    return;
  }
  try {
    // remove myself
    for (std::vector<std::pair<std::string,int> >::iterator it = ret.begin(); it != ret.end(); it++) {
      if (it->first == argv().eth && it->second == argv().port) {
        it = ret.erase(it);
        it--;
        continue;
      }
    }
  } catch(...) {
    // eliminate the exception "no clients."
  }
#endif
}

}
}
