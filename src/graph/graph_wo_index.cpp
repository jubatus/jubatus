#include <pficommon/lang/cast.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/serialization.h>
#include "graph_wo_index.hpp"

using namespace std;
using namespace pfi::data;
using namespace pfi::lang;

namespace jubatus {
namespace graph {

graph_wo_index::graph_wo_index(){
  clear();
}

graph_wo_index::~graph_wo_index(){
}

void graph_wo_index::clear(){
  local_nodes_.clear();
  local_edges_.clear();
  spts_.clear();
  spts_.resize(LANDMARK_NUM);
}

void graph_wo_index::create_node(node_id_t id){
  local_nodes_[id] = node_info();
}

void graph_wo_index::create_global_node(node_id_t id){
  global_nodes_[id] = 0;
}

void graph_wo_index::remove_global_node(node_id_t id){
}

void graph_wo_index::update_node(node_id_t id, const property& p){
  local_nodes_[id].p = p;
}

void graph_wo_index::remove_node(node_id_t id){
  
}

void graph_wo_index::create_edge(edge_id_t eid, node_id_t src, node_id_t tgt){
  edge_info ei;
  ei.src = src;
  ei.tgt = tgt;
  local_edges_[eid] = ei;
  local_nodes_[src].out_edges.push_back(eid);
  local_nodes_[tgt].in_edges.push_back(eid);
}

void graph_wo_index::update_edge(edge_id_t eid, const property& p){
  local_edges_[eid].p = p;
}

void graph_wo_index::remove_edge(edge_id_t eid){
}
  
double graph_wo_index::centerality(node_id_t id, centerality_type ct) const{
  if (ct == EIGENSCORE){
    unordered_map<node_id_t, eigen_vector_info>::const_iterator it = eigen_scores_.find(id);
    if (it == eigen_scores_.end()){
      throw runtime_error(string("graph_wo_index::centerality id=")
                          + lexical_cast<string>(id));
    }
    return it->second.score;
  } else {
    throw runtime_error(string("graph_wo_index::centerality not supported type ct=")
                        + lexical_cast<string>(ct));
  }

}

void graph_wo_index::shortest_path(node_id_t src, node_id_t tgt,
                                   uint64_t max_hop, std::vector<node_id_t>& ret) const{
  ret.clear();
  uint64_t min_score = LONG_LONG_MAX;
  uint64_t ind = LONG_LONG_MAX;
  for (uint64_t i = 0; i < LANDMARK_NUM; ++i){
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
    if (to_root.find(cur) == to_root.end()){
      ret.clear();
      return;
    }
    ret.push_back(cur);
  }

  for (uint64_t cur = landmark; cur != tgt; ){
    if (from_root.find(cur) == from_root.end()){
      ret.clear();
      return;
    }
    ret.push_back(cur);
  }
}
  
void graph_wo_index::get_node(node_id_t id, node_info& ret) const{
  pfi::data::unordered_map<node_id_t, node_info>::const_iterator it = local_nodes_.find(id);
  if (it == local_nodes_.end()){
    throw runtime_error(string("graph_wo_idnex::get_node id=" ) 
                        + pfi::lang::lexical_cast<string>(id));
  }
  ret = it->second;
}

void graph_wo_index::get_edge(edge_id_t eid, edge_info& ret) const{
  pfi::data::unordered_map<edge_id_t, edge_info>::const_iterator it = local_edges_.find(eid);
  if (it == local_edges_.end()){
    throw runtime_error(string("graph_wo_idnex::get_edge eid=" ) 
                        + pfi::lang::lexical_cast<string>(eid));
  }
  ret = it->second;
}

string graph_wo_index::type() const {
  return string("graph_wo_index");
}

bool graph_wo_index::save_imp(std::ostream& os){
  pfi::data::serialization::binary_oarchive oa(os);
  oa << local_nodes_
     << local_edges_
     << global_nodes_
     << eigen_scores_
     << spts_;
  return true;
}

bool graph_wo_index::load_imp(std::istream& is){
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
  std::vector<edge_id_t> in_edges;
  std::vector<edge_id_t> out_edges;
};

struct edge_info{
  property p;
  node_id_t src;
  node_id_t tgt;
};
*/

void graph_wo_index::update_index(){
}

void graph_wo_index::get_diff_eigen_score(eigen_vector_diff& diff) {
  diff.clear(); // tmp_diff + swap is better ?
  for (pfi::data::unordered_map<node_id_t, node_info>::const_iterator it = local_nodes_.begin();
       it != local_nodes_.end(); ++it){
    const vector<edge_id_t>& in_edges = it->second.in_edges;
    double score = 0.0;
    for (size_t i = 0; i < in_edges.size(); ++i){
      edge_info ei = local_edges_[in_edges[i]]; // should be const
      double src_score = eigen_scores_[ei.src].score;
      score += src_score / eigen_scores_[ei.src].out_degree_num;
    }
    eigen_vector_info ei;
    ei.score = alpha_ * score + 1 - alpha_;
    ei.out_degree_num = it->second.out_edges.size();
    diff[it->first] = ei;
  }
}

void graph_wo_index::set_mixed_and_clear_diff_eigen_score(eigen_vector_mixed& mixed){
  eigen_scores_ = mixed;
}

void graph_wo_index::update_spt_edges(spt_edges& se, node_id_t landmark, bool is_out){
  se[landmark] = make_pair(0, landmark);
  for (pfi::data::unordered_map<node_id_t, node_info>::const_iterator it = local_nodes_.begin();
       it != local_nodes_.end(); ++it){
    if (it->first == landmark) {
      continue;
    }
    const vector<edge_id_t>& edges = (is_out) ? it->second.out_edges : it->second.in_edges;
    pair<uint64_t, node_id_t> best_parent = make_pair(LONG_LONG_MAX, -1);
    for (vector<edge_id_t>::const_iterator it2 = edges.begin(); it2 != edges.end(); ++it2){
      node_id_t tgt = local_edges_[*it2].tgt;
      spt_edges::const_iterator tr_it = se.find(tgt);
      if (tr_it != se.end()){
        best_parent = min(best_parent, make_pair(tr_it->second.first + 1, tgt));
      }
    }
    se[it->first] = best_parent;
  }
}

void graph_wo_index::update_spt(){
  for (uint64_t i = 0; i < LANDMARK_NUM; ++i){
    shortest_path_tree& spt = spts_[i];
    if (spt.landmark == LONG_LONG_MAX) continue;
    update_spt_edges(spt.to_root, spt.landmark, true);
    update_spt_edges(spt.from_root, spt.landmark, false);
  }
}

void graph_wo_index::get_diff_shortest_path_tree(spt_diff& diff){
  diff.clear();
  diff.resize(LANDMARK_NUM);
  for (uint64_t i = 0; i < LANDMARK_NUM; ++i){
    const shortest_path_tree& spt = spts_[i];
    if (spt.landmark == LONG_LONG_MAX) continue;
    for (pfi::data::unordered_map<node_id_t, node_info>::const_iterator it = local_nodes_.begin();
         it != local_nodes_.end(); ++it){
      node_id_t id = it->first;

      spt_edges::const_iterator from_it = spt.from_root.find(id);
      if (from_it != spt.from_root.end()){
        diff[i].from_root[id] = from_it->second;
      }

      spt_edges::const_iterator to_it = spt.to_root.find(id);
      if (from_it != spt.to_root.end()){
        diff[i].to_root[id] = to_it->second;
      }
    }
  }
}

void graph_wo_index::set_mixed_and_clear_diff_shortest_path_tree(const spt_mixed& mixed){
  spts_ = mixed;
}

void graph_wo_index::get_diff(string& diff){
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

void graph_wo_index::set_mixed_and_clear_diff(string& mixed){
  istringstream is(mixed);
  pfi::data::serialization::binary_iarchive ia(is);
  
  eigen_vector_mixed emixed;
  ia >> emixed;
  set_mixed_and_clear_diff_eigen_score(emixed);

  spt_mixed smixed;
  ia >> smixed;
  set_mixed_and_clear_diff_shortest_path_tree(smixed);
}

void graph_wo_index::get_status(std::map<std::string, std::string>& status) const {
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
    if (diff[i].landmark == LONG_LONG_MAX) continue;
    diff_landmark2ind[diff[i].landmark] = i;
  }

  map<node_id_t, uint64_t> mixed_landmark2ind;
  uint64_t set_num = LANDMARK_NUM;
  for (uint64_t i = 0; i < mixed.size(); ++i){
    if (mixed[i].landmark == LONG_LONG_MAX) {
      set_num = i;
      return;
    }
    mixed_landmark2ind[mixed[i].landmark] = i;
  }

  for (map<node_id_t, uint64_t>::const_iterator it = diff_landmark2ind.begin(); set_num < LANDMARK_NUM && it != diff_landmark2ind.end(); ++it){
    
  }

  for (map<node_id_t, uint64_t>::const_iterator it = diff_landmark2ind.begin(); it != diff_landmark2ind.end(); ++it){
    if (mixed_landmark2ind.find(it->first) != mixed_landmark2ind.end()){
      mix_spt(diff[it->second], mixed[mixed_landmark2ind[it->first]]);
    } else {
      mixed[set_num++] = diff[it->second];
    }
  }

}

}
}
