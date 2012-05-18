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

#include <cassert>
#include <pficommon/lang/cast.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/serialization.h>
#include "graph_wo_index.hpp"

using namespace std;
using namespace pfi::data;
using namespace pfi::lang;

namespace jubatus {
namespace graph {

graph_wo_index::graph_wo_index() : alpha_(0.9){
  clear();
}

graph_wo_index::~graph_wo_index(){
}

void graph_wo_index::clear(){
  local_nodes_.clear();
  local_edges_.clear();
  global_nodes_.clear();
  eigen_scores_.clear();
  spts_.clear();
}

void graph_wo_index::create_node(node_id_t id){
  if (local_nodes_.count(id) > 0){
    throw runtime_error(string("graph_wo_index::create_node already exist id=")
                        + lexical_cast<string>(id));
  }
  local_nodes_[id] = node_info();
  may_set_landmark(id);
}

void graph_wo_index::may_set_landmark(node_id_t id){
  if (id>1) return;
  if (spts_.size() == LANDMARK_NUM) return;
  shortest_path_tree spt;
  spt.landmark = id;
  spts_.push_back(spt);
}

void graph_wo_index::create_global_node(node_id_t id){
  if (global_nodes_.count(id) > 0){
    throw runtime_error(string("graph_wo_index::create_global_node already exist id=")
                        + lexical_cast<string>(id));
  }
  global_nodes_[id] = 0;
}

void graph_wo_index::remove_global_node(node_id_t id){
  if (global_nodes_.count(id) == 0){
    throw runtime_error(string("graph_wo_index::remove_global_node unknown id=" ) 
                        + pfi::lang::lexical_cast<string>(id));
  }
  global_nodes_.erase(id);
}

void graph_wo_index::update_node(node_id_t id, const property& p){
  pfi::data::unordered_map<node_id_t, node_info>::iterator it = local_nodes_.find(id);
  if (it == local_nodes_.end()){
    throw runtime_error(string("graph_wo_index::update_node unknown id=" ) 
                        + pfi::lang::lexical_cast<string>(id));
  }
  it->second.p = p;
}

void graph_wo_index::remove_node(node_id_t id){
  node_info ni;
  try {
    get_node(id, ni);
  } catch (runtime_error& e){
    throw runtime_error(string("graph_wo_index::remove_node unknown id=")
                        + lexical_cast<string>(id) + " " + e.what());
  }
  if (ni.in_edges.size() > 0 || ni.out_edges.size() > 0){
    throw runtime_error(string("graph_wo_index::remove_node unknown id=")
                        + lexical_cast<string>(id));
  }
  local_nodes_.erase(id);
}

void graph_wo_index::create_edge(edge_id_t eid, node_id_t src, node_id_t tgt){
  if (local_nodes_.find(src) == local_nodes_.end() &&
      local_nodes_.find(tgt) == local_nodes_.end()){
    throw runtime_error(string("graph_wo_index::create_edge unknown src id=")
                        + lexical_cast<string>(src) + " tgt id="
                        + lexical_cast<string>(tgt));
  }
  if (local_edges_.count(eid) > 0){
    throw runtime_error(string("graph_wo_index::create_edge already exist id=")
                        + lexical_cast<string>(eid));
  }
  
  edge_info ei;
  ei.src = src;
  ei.tgt = tgt;
  local_edges_[eid] = ei;
  if (local_nodes_.count(src) > 0){
    local_nodes_[src].out_edges.push_back(eid);
  }
  if (local_nodes_.count(tgt) > 0){
    local_nodes_[tgt].in_edges.push_back(eid);
  }
}

void graph_wo_index::update_edge(edge_id_t eid, const property& p){
  pfi::data::unordered_map<edge_id_t, edge_info>::iterator it = local_edges_.find(eid);
  if (it == local_edges_.end()){
    throw runtime_error(string("graph_wo_index::update_edge unknown id =" ) 
                        + pfi::lang::lexical_cast<string>(eid));
  }
  it->second.p = p;
}

void graph_wo_index::remove_edge(edge_id_t eid){
  pfi::data::unordered_map<edge_id_t, edge_info>::iterator it = local_edges_.find(eid);
  if (it == local_edges_.end()){
    throw runtime_error(string("graph_wo_index::remove_edge unknown id =" ) 
                        + pfi::lang::lexical_cast<string>(eid));
  }
  node_id_t src = it->second.src;
  node_id_t tgt = it->second.tgt;

  if (local_nodes_.count(src) > 0){
    remove_by_swap(local_nodes_[src].out_edges, eid);
  }
  if (local_nodes_.count(tgt) > 0){
    remove_by_swap(local_nodes_[tgt].in_edges, eid);
  }
  
  local_edges_.erase(it);
}
  
void graph_wo_index::add_cenrality_query(const preset_query&)
{}
void graph_wo_index::add_shortest_path_query(const preset_query&)
{}
void graph_wo_index::remove_cenrality_query(const preset_query&)
{}
void graph_wo_index::remove_shortest_path_query(const preset_query&)
{}


double graph_wo_index::centrality(node_id_t id, centrality_type ct, const preset_query&) const{

  if (ct == EIGENSCORE){
    unordered_map<node_id_t, eigen_vector_info>::const_iterator it = eigen_scores_.find(id);
    if (it == eigen_scores_.end()){
      throw runtime_error(string("graph_wo_index::centrality unknown id=")
                          + lexical_cast<string>(id));
    }
    return it->second.score;
  } else {
    throw runtime_error(string("graph_wo_index::centrality unknown type ct=")
                        + lexical_cast<string>(ct));
  }

}

void graph_wo_index::shortest_path(node_id_t src, node_id_t tgt,
                                   uint64_t max_hop, vector<node_id_t>& ret,
				   const preset_query&) const{
  if (global_nodes_.count(src) == 0){
    throw runtime_error(string("graph_wo_index::shortest_path unknown id=")
                        + lexical_cast<string>(src));
  }
  if (global_nodes_.count(tgt) == 0){
    throw runtime_error(string("graph_wo_index::shortest_path unknown id=")
                        + lexical_cast<string>(tgt));
  }
  ret.clear();
  uint64_t min_score = LONG_LONG_MAX;
  uint64_t ind = LONG_LONG_MAX;
  for (uint64_t i = 0; i < spts_.size(); ++i){
    const shortest_path_tree& spt = spts_[i];
    spt_edges::const_iterator src_it = spt.to_root.find(src);
    spt_edges::const_iterator tgt_it = spt.from_root.find(tgt);
    if (src_it == spt.to_root.end() ||tgt_it == spt.from_root.end()){
      continue;
    }
    uint64_t score = src_it->second.first + tgt_it->second.first;
    if (score < min_score){
      min_score = score;
      ind = i;
    }
  }

  if (ind >= LANDMARK_NUM){
    return;
  }

  
  const spt_edges& to_root = spts_[ind].to_root;
  const spt_edges& from_root = spts_[ind].from_root;
  uint64_t landmark = spts_[ind].landmark;


  for (uint64_t cur = src; cur != landmark; ){
    if (ret.size() >= max_hop){
      return;
    }
    spt_edges::const_iterator it = to_root.find(cur);
    if (it == to_root.end()){
      ret.clear();
      return;
    }
    ret.push_back(cur);
    cur = it->second.second;
  }
  ret.push_back(landmark);

  vector<node_id_t> from_root_path;
  for (uint64_t cur = tgt; cur != landmark; ){
    if (ret.size() + from_root_path.size() >= max_hop){
      return;
    }
    spt_edges::const_iterator it = from_root.find(cur);
    if (it == from_root.end()){
      ret.clear();
      return;
    }
    from_root_path.push_back(cur);
    cur = it->second.second;
  }
  
  for (vector<node_id_t>::const_reverse_iterator rit = from_root_path.rbegin(); rit != from_root_path.rend(); ++rit){
    ret.push_back(*rit);
  }
}
  
void graph_wo_index::get_node(node_id_t id, node_info& ret) const{
  pfi::data::unordered_map<node_id_t, node_info>::const_iterator it = local_nodes_.find(id);
  if (it == local_nodes_.end()){
    throw runtime_error(string("graph_wo_index::get_node unknown id=" ) 
                        + pfi::lang::lexical_cast<string>(id));
  }
  ret = it->second;
}

void graph_wo_index::get_edge(edge_id_t eid, edge_info& ret) const{
  pfi::data::unordered_map<edge_id_t, edge_info>::const_iterator it = local_edges_.find(eid);
  if (it == local_edges_.end()){
    throw runtime_error(string("graph_wo_index::get_edge unknown id=" ) 
                        + pfi::lang::lexical_cast<string>(eid));
  }
  ret = it->second;
}

string graph_wo_index::type() const {
  return string("graph_wo_index");
}

bool graph_wo_index::save_imp(ostream& os){
  pfi::data::serialization::binary_oarchive oa(os);
  oa << local_nodes_
     << local_edges_
     << global_nodes_
     << eigen_scores_
     << spts_;
  return true;
}

bool graph_wo_index::load_imp(istream& is){
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> local_nodes_
     >> local_edges_
     >> global_nodes_
     >> eigen_scores_
     >> spts_;
  return true;
}

/*
struct node_info{
  property p;
  vector<edge_id_t> in_edges;
  vector<edge_id_t> out_edges;
};

struct edge_info{
  property p;
  node_id_t src;
  node_id_t tgt;
};
*/

void graph_wo_index::update_index(){
  update_spt();
}

void graph_wo_index::get_diff_eigen_score(eigen_vector_diff& diff) const {
  diff.clear(); // tmp_diff + swap is better ?

  double dist = 0;
  for (pfi::data::unordered_map<node_id_t, eigen_vector_info>::const_iterator it = eigen_scores_.begin();
       it != eigen_scores_.end(); ++it){
    if (it->second.out_degree_num == 0){
      dist += it->second.score;
    }
  }

  uint64_t new_node_num = 0;
  double dist_from_new_node = 0.0;
  for (pfi::data::unordered_map<node_id_t, node_info>::const_iterator it = local_nodes_.begin(); it != local_nodes_.end(); ++it){
    if (eigen_scores_.count(it->first) == 0){
      dist_from_new_node += 1.0;
      ++new_node_num;
    }
  }
  dist += dist_from_new_node;
  
  if (eigen_scores_.size() + new_node_num > 0){
    dist /= (eigen_scores_.size() + new_node_num);
  }

  for (pfi::data::unordered_map<node_id_t, node_info>::const_iterator it = local_nodes_.begin(); it != local_nodes_.end(); ++it){
    const vector<edge_id_t>& in_edges = it->second.in_edges;
    double score = 0.0;
    for (size_t i = 0; i < in_edges.size(); ++i){
      pfi::data::unordered_map<edge_id_t, edge_info>::const_iterator it0;
      it0 = local_edges_.find(in_edges[i]);
      if(it0 == local_edges_.end()){
	continue;
      }
      pfi::data::unordered_map<node_id_t, eigen_vector_info>::const_iterator it1;      
      it1 = eigen_scores_.find(it0->second.src);
      if(it1 == eigen_scores_.end()){
	continue;
      }
      if (it1->second.out_degree_num != 0){
        // FIX ME !!!! it->second.score > 0 should indicate it->second.out_degree_num
        score += it1->second.score / it1->second.out_degree_num;

      }
    }
    eigen_vector_info ei;
    ei.score = alpha_ * score + 1 - alpha_ + alpha_ * dist;
    ei.out_degree_num = it->second.out_edges.size();
    diff[it->first] = ei;
  }
}

void graph_wo_index::set_mixed_and_clear_diff_eigen_score(eigen_vector_mixed& mixed){
  eigen_scores_ = mixed;
  if (eigen_scores_.size() == 0){
    return;
  }

  // normalize
  double sum = 0.0;
  for (pfi::data::unordered_map<node_id_t, eigen_vector_info>::const_iterator it = eigen_scores_.begin(); it != eigen_scores_.end(); ++it){
    sum += it->second.score;
  }
  double normalizer = eigen_scores_.size() / sum;
  for (pfi::data::unordered_map<node_id_t, eigen_vector_info>::iterator it = eigen_scores_.begin(); it != eigen_scores_.end(); ++it){
    it->second.score *= normalizer;
  }
}

void graph_wo_index::update_spt_edges(spt_edges& se, node_id_t landmark, bool is_out){
  se[landmark] = make_pair(0, landmark);

  if (is_out) {
    for (pfi::data::unordered_map<node_id_t, node_info>::const_iterator it = local_nodes_.begin();
         it != local_nodes_.end(); ++it){
      
      const vector<edge_id_t>& edges = it->second.out_edges;
      for (vector<edge_id_t>::const_iterator it2 = edges.begin(); it2 != edges.end(); ++it2) {
        node_id_t from = local_edges_[*it2].src;
        node_id_t to = local_edges_[*it2].tgt;
        
        spt_edges::iterator tr_it = se.find(from);
        if (tr_it != se.end()) {
          uint64_t dist = tr_it->second.first + 1;
          spt_edges::iterator tr_jt = se.find(to);
          if (tr_jt != se.end()) {
            if (dist < tr_jt->second.first) {
              tr_jt->second.first = dist;
              tr_jt->second.second = from;
            }
          }
          else {
            se.insert(make_pair(to, make_pair(dist, from)));
          }
        }
      }
    }
  }
  else {
    for (pfi::data::unordered_map<node_id_t, node_info>::const_iterator it = local_nodes_.begin();
         it != local_nodes_.end(); ++it){
      
      const vector<edge_id_t>& edges = it->second.in_edges;
      for (vector<edge_id_t>::const_iterator it2 = edges.begin(); it2 != edges.end(); ++it2) {
        node_id_t from = local_edges_[*it2].src;
        node_id_t to = local_edges_[*it2].tgt;
        
        spt_edges::iterator tr_it = se.find(to);
        if (tr_it != se.end()) {
          uint64_t dist = tr_it->second.first + 1;
          spt_edges::iterator tr_jt = se.find(from);
          if (tr_jt != se.end()) {
            if (dist < tr_jt->second.first) {
              tr_jt->second.first = dist;
              tr_jt->second.second = to;
            }
          }
          else {
            se.insert(make_pair(from, make_pair(dist, to)));
          }
        }
      }
    }
  }
}

void graph_wo_index::update_spt(){
  for (uint64_t i = 0; i < spts_.size(); ++i){
    shortest_path_tree& spt = spts_[i];
    update_spt_edges(spt.to_root, spt.landmark, false);
    update_spt_edges(spt.from_root, spt.landmark, true);
  }
}

void graph_wo_index::get_diff_shortest_path_tree(spt_diff& diff)const{
  diff.clear();
  diff.resize(spts_.size());
  for (uint64_t i = 0; i < spts_.size(); ++i){
    const shortest_path_tree& spt = spts_[i];
    if (spt.landmark == LONG_LONG_MAX) continue;
    for (pfi::data::unordered_map<node_id_t, node_info>::const_iterator it = local_nodes_.begin();
         it != local_nodes_.end(); ++it){
      node_id_t id = it->first;
      diff[i].landmark = spt.landmark;

      spt_edges::const_iterator from_it = spt.from_root.find(id);
      if (from_it != spt.from_root.end()){
        diff[i].from_root[id] = from_it->second;
      }

      spt_edges::const_iterator to_it = spt.to_root.find(id);
      if (to_it != spt.to_root.end()){
        diff[i].to_root[id] = to_it->second;
      }
    }
  }
}

void graph_wo_index::set_mixed_and_clear_diff_shortest_path_tree(const spt_mixed& mixed){
  spts_ = mixed;
}

void graph_wo_index::get_diff(string& diff)const{
  const_cast<graph_wo_index*>(this)->update_spt(); // get_diff should be constant

  ostringstream os;
  pfi::data::serialization::binary_oarchive oa(os);

  eigen_vector_diff ev_diff;
  get_diff_eigen_score(ev_diff);
  oa << ev_diff;

  spt_diff sdiff;
  get_diff_shortest_path_tree(sdiff);
  oa << sdiff;
  diff = os.str();
}

void graph_wo_index::set_mixed_and_clear_diff(const string& mixed){
  istringstream is(mixed);
  pfi::data::serialization::binary_iarchive ia(is);
  
  eigen_vector_mixed emixed;
  ia >> emixed;
  set_mixed_and_clear_diff_eigen_score(emixed);

  spt_mixed smixed;
  ia >> smixed;
  set_mixed_and_clear_diff_shortest_path_tree(smixed);
}

void graph_wo_index::get_status(map<string, string>& status) const {
  status["local_node_num"] = lexical_cast<string>(local_nodes_.size());
  status["global_node_num"] = lexical_cast<string>(global_nodes_.size());
  status["local_edge_num"] = lexical_cast<string>(local_edges_.size());
}

void graph_wo_index::mix(const string& diff, string& mixed){
  eigen_vector_diff ediff;
  spt_diff sdiff;
  {
    istringstream is_diff(diff);
    pfi::data::serialization::binary_iarchive ia(is_diff);
    ia >> ediff;
    ia >> sdiff;
  }
  
  eigen_vector_mixed emixed;
  spt_mixed smixed;
  {
    istringstream is_mixed(mixed);
    pfi::data::serialization::binary_iarchive ia(is_mixed);
    ia >> emixed;
    ia >> smixed;
  }

  mix(ediff, emixed);
  mix(sdiff, smixed);

  ostringstream os;
  pfi::data::serialization::binary_oarchive oa(os);

  oa << emixed
     << smixed;
  mixed = os.str();
}

void graph_wo_index::mix(const eigen_vector_diff& diff, eigen_vector_mixed& mixed){
  for (eigen_vector_diff::const_iterator it = diff.begin(); it != diff.end(); ++it){
    mixed[it->first] = it->second;
  }
}

void  graph_wo_index::mix_spt(const shortest_path_tree& diff, shortest_path_tree& mixed){
  for (spt_edges::const_iterator it = diff.from_root.begin(); it != diff.from_root.end(); ++it){
    mixed.from_root[it->first] = it->second;
  }

  for (spt_edges::const_iterator it = diff.to_root.begin(); it != diff.to_root.end(); ++it){
    mixed.to_root[it->first] = it->second;
  }
}

void graph_wo_index::mix(const spt_diff& diff, spt_mixed& mixed){
  map<node_id_t, uint64_t> diff_landmark2ind;
  for (uint64_t i = 0; i < diff.size(); ++i){
    if (diff[i].landmark != LONG_LONG_MAX) {
      diff_landmark2ind[diff[i].landmark] = i;
    }
  }

  map<node_id_t, uint64_t> mixed_landmark2ind;
  for (uint64_t i = 0; i < mixed.size(); ++i){
    if (mixed[i].landmark != LONG_LONG_MAX) {
      mixed_landmark2ind[mixed[i].landmark] = i;
    }
  }

  for (map<node_id_t, uint64_t>::const_iterator it = diff_landmark2ind.begin(); it != diff_landmark2ind.end(); ++it){
    if (mixed_landmark2ind.find(it->first) != mixed_landmark2ind.end()){
      mix_spt(diff[it->second], mixed[mixed_landmark2ind[it->first]]);
    } else if (mixed.size() < LANDMARK_NUM){
      mixed.push_back(diff[it->second]);
    }
  }
}

void graph_wo_index::remove_by_swap(std::vector<edge_id_t>& edges, edge_id_t eid){
  for (size_t i = 0; i < edges.size(); ++i){
    if (edges[i] == eid){ 
      if (i+1 < edges.size()){
        swap(edges[i], edges.back());
      }
      edges.resize(edges.size()-1);
      return;
    }
  }
}

}
}
