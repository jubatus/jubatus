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

#include "graph_serv.hpp"
#include "graph_types.hpp"

#include "../graph/graph_factory.hpp"
#include "../common/util.hpp"

#include <pficommon/lang/cast.h>
#include <sstream>

namespace jubatus { namespace server {


inline node_id uint642nodeid(uint64_t i){
  return pfi::lang::lexical_cast<node_id, uint64_t>(i);
}
inline uint64_t nodeid2uint64(const node_id& id)
{
  return pfi::lang::lexical_cast<uint64_t, node_id>(id);
}

inline node_id i2n(uint64_t i){
  return uint642nodeid(i);
}
inline uint64_t n2i(const node_id& id){
  return nodeid2uint64(id);
}

graph_serv::graph_serv(const framework::server_argv& a)
  : jubatus_serv(a)
{
  pfi::lang::shared_ptr<jubatus::graph::graph_base> 
    g(jubatus::graph::create_graph("graph_wo_index"));
  register_mixable(mixable_cast(&g_));
}
graph_serv::~graph_serv()
{}

std::string graph_serv::create_node(){
  uint64_t nid = jubatus::util::new_uid();
  // send true create_node_ to other machine,
  // send true create_global_node to other machines.

  // TODO: global registration
  std::vector<std::pair<std::string, int> > members;
  get_members(members);
  //g_.get_model()->create_node(nid);
  // common::mprpc::multi_rsync r(members);
  // r.call<bool>(agg);
  return pfi::lang::lexical_cast<std::string>(nid);
}


int graph_serv::update_node(const std::string& id, const property& p)
{
  g_.get_model()->update_node(n2i(id), p);
  return 0;
}

int graph_serv::remove_node(const std::string& id){
  g_.get_model()->remove_node(n2i(id));
  std::vector<std::pair<std::string, int> > members;
  get_members(members);
  //g_.get_model()->create_node(nid);
  // common::mprpc::multi_rsync r(members);
  // r.call<bool>(agg);
  return 0;
}

  //@cht
int graph_serv::create_edge(const std::string& id, const edge_info& ei)
{ 
  edge_id_t eid = jubatus::util::new_uid();
  g_.get_model()->create_edge(eid, n2i(ei.src), n2i(ei.tgt));
  //g_.get_model()->update_edge(id, ei.p);
  return 0;
}

  //@random
int graph_serv::update_edge(const std::string&, edge_id_t eid, const edge_info& ei)
{
  g_.get_model()->update_edge(eid, ei.p);
  return 0;
}


int graph_serv::remove_edge(const std::string&, const edge_id_t& id){
  g_.get_model()->remove_edge(id);
  return 0;
}

//@random
double graph_serv::centrality(const std::string& id, const centrality_type& s,
			      const preset_query& q) const 
{ 
  if(s == 0){
    jubatus::graph::preset_query q0;
    return g_.get_model()->centrality(n2i(id),
				      jubatus::graph::EIGENSCORE,
				      q0);
  }else{
    std::stringstream msg;
    msg << "unknown centrality type: " << s;
    LOG(ERROR) << msg.str();
    throw std::runtime_error(msg.str());
  }
  
}
//@random
std::vector<node_id> graph_serv::shortest_path(const shortest_path_req& req) const
{ 
  std::vector<jubatus::graph::node_id_t> ret0;
  jubatus::graph::preset_query q;
  g_.get_model()->shortest_path(n2i(req.src), n2i(req.tgt), req.max_hop, ret0, q);
  std::vector<node_id> ret;
  for(size_t i=0;i<ret0.size();++i){
    ret.push_back(i2n(ret0[i]));
  }
  return ret;
}

//update, broadcast
bool graph_serv::add_cenrality_query(const preset_query& q)
{
  return false;
}

//update, broadcast
bool graph_serv::add_shortest_path_query(const preset_query& q)
{
  return false;
}

//update, broadcast
bool graph_serv::remove_cenrality_query(const preset_query& q)
{
  return false;
}

//update, broadcast
bool graph_serv::remove_shortest_path_query(const preset_query& q)
{
  return false;
}

node_info graph_serv::get_node(const std::string& nid)const
{
  jubatus::graph::node_info info;
  g_.get_model()->get_node(n2i(nid), info);
  jubatus::node_info ret;
  framework::convert<graph::node_info, jubatus::node_info>(info, ret);
  return ret;
}
//@random
edge_info graph_serv::get_edge(const std::string& nid, const edge_id_t& id)const
{
  jubatus::graph::edge_info info;
  g_.get_model()->get_edge((jubatus::graph::edge_id_t)id, info);
  jubatus::edge_info ret;
  ret.p = info.p;
  ret.src = i2n(info.src);
  ret.tgt = i2n(info.tgt);
  return ret;
}

//@broadcast
int graph_serv::update_index(){
  g_.get_model()->update_index();
  std::string diff;                                                                    
  g_.get_model()->get_diff(diff);
  g_.get_model()->set_mixed_and_clear_diff(diff);
  return 0;
}
int graph_serv::clear(){
  g_.get_model()->clear();
  return 0;
}

std::map<std::string, std::map<std::string,std::string> > graph_serv::get_status()const
{
  std::map<std::string,std::string> ret0;

  g_.get_model()->get_status(ret0);

  std::map<std::string, std::map<std::string,std::string> > ret =
    jubatus_serv::get_status();

  ret[get_server_identifier()].insert(ret0.begin(), ret0.end());
  return ret;
}

int graph_serv::create_global_node(const std::string& nid)
{
  g_.get_model()->create_global_node(n2i(nid));
  return 0;
} //update internal

int graph_serv::remove_global_node(const std::string& nid)
{
  g_.get_model()->remove_global_node(n2i(nid));
  return 0;
} //update internal

void graph_serv::after_load(){}
}
}
