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

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/unordered_set.h>
#include <pficommon/lang/cast.h>

#include "graph_wo_index.hpp"

using std::endl;
using std::map;
using std::pair;
using std::string;
using std::swap;
using std::vector;
using pfi::lang::lexical_cast;
using pfi::data::unordered_set;

namespace jubatus {
namespace core {
namespace graph {

namespace {

// Debug print
void print_tree(const shortest_path_tree& spt, std::ostream& out) {
  out << "landmark = " << spt.landmark << std::endl;
  out << "from root = [";
  for (spt_edges::const_iterator it = spt.from_root.begin();
       it != spt.from_root.end(); ++it) {
    if (it != spt.from_root.begin()) {
      out << ", ";
    }
    out << '(' << it->first << ", (" << it->second.first << ", "
        << it->second.second << "))";
  }
  out << "]\n";
  out << "to root = [";
  for (spt_edges::const_iterator it = spt.to_root.begin();
       it != spt.to_root.end(); ++it) {
    if (it != spt.to_root.begin()) {
      out << ", ";
    }
    out << '(' << it->first << ", (" << it->second.first << ", "
        << it->second.second << "))";
  }
  out << "]" << endl;
}

bool is_empty_query(const preset_query& query) {
  return query.node_query.empty() && query.edge_query.empty();
}

bool is_matched_to_query(
    const vector<pair<string, string> >& query,
    const property& prop) {
  for (size_t i = 0; i < query.size(); ++i) {
    property::const_iterator it = prop.find(query[i].first);
    if (it == prop.end() || it->second != query[i].second) {
      return false;
    }
  }
  return true;
}

void normalize(eigen_vector_mixed& v) {
  double sum = 0;
  for (eigen_vector_mixed::const_iterator it = v.begin(); it != v.end(); ++it) {
    sum += it->second.score;
  }
  const double normalizer = v.size() / sum;
  for (eigen_vector_mixed::iterator it = v.begin(); it != v.end(); ++it) {
    it->second.score *= normalizer;
  }
}

}  // namespace

graph_wo_index::graph_wo_index(const config& config)
    : config_(config) {
  clear();
}

graph_wo_index::graph_wo_index() {
  clear();
}

graph_wo_index::~graph_wo_index() {
}

void graph_wo_index::alpha(double a) {
  config_.alpha = a;
}

void graph_wo_index::clear() {
  local_nodes_.clear();
  local_edges_.clear();
  global_nodes_.clear();
  eigen_scores_.clear();
  spts_.clear();
}

void graph_wo_index::create_node(node_id_t id) {
  if (local_nodes_.count(id) > 0) {
    throw JUBATUS_EXCEPTION(local_node_exists(id));
  }
  local_nodes_[id] = node_info();
  may_set_landmark(id);
}

void graph_wo_index::may_set_landmark(node_id_t id) {
  // Do we need it?
  // if (id > 1) return;
  for (spt_query_mixed::iterator it = spts_.begin(); it != spts_.end(); ++it) {
    spt_mixed& mixed = it->second;
    if (mixed.size() == static_cast<size_t>(config_.landmark_num)
        || !is_node_matched_to_query(it->first, id)) {
      return;
    }

    shortest_path_tree spt;
    spt.landmark = id;
    mixed.push_back(spt);
  }
}

void graph_wo_index::create_global_node(node_id_t id) {
  if (global_nodes_.count(id) > 0) {
    throw JUBATUS_EXCEPTION(global_node_exists(id));
  }
  global_nodes_[id] = 0;
}

void graph_wo_index::remove_global_node(node_id_t id) {
  if (global_nodes_.count(id) == 0) {
    throw JUBATUS_EXCEPTION(unknown_id("remove_global_node", id));
  }
  global_nodes_.erase(id);
}

void graph_wo_index::update_node(node_id_t id, const property& p) {
  node_info_map::iterator it = local_nodes_.find(id);
  if (it == local_nodes_.end()) {
    throw JUBATUS_EXCEPTION(unknown_id("update_node", id));
  }
  it->second.p = p;
  may_set_landmark(id);
}

void graph_wo_index::remove_node(node_id_t id) {
  node_info ni;
  try {
    get_node(id, ni);
  } catch (jubatus::exception::runtime_error&) {
    throw JUBATUS_EXCEPTION(unknown_id("remove_node", id));
  }
  if (ni.in_edges.size() > 0 || ni.out_edges.size() > 0) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(
        string("graph_wo_index::remove_node unknown id=")
        + lexical_cast<string>(id)));
  }
  local_nodes_.erase(id);
}

void graph_wo_index::create_edge(edge_id_t eid, node_id_t src, node_id_t tgt) {
  if (local_nodes_.find(src) == local_nodes_.end()
      && local_nodes_.find(tgt) == local_nodes_.end()) {
    throw JUBATUS_EXCEPTION(
        unknown_id(
            string("graph_wo_index::create_edge unknown src id=")
            + lexical_cast<string>(src)
            + " tgt id=" + lexical_cast<string>(tgt),
            src));
  }
  if (local_edges_.count(eid) > 0) {
    throw JUBATUS_EXCEPTION(edge_exists(eid));
  }

  edge_info ei;
  ei.src = src;
  ei.tgt = tgt;
  local_edges_[eid] = ei;
  if (local_nodes_.count(src) > 0) {
    local_nodes_[src].out_edges.push_back(eid);
  }
  if (local_nodes_.count(tgt) > 0) {
    local_nodes_[tgt].in_edges.push_back(eid);
  }
}

void graph_wo_index::update_edge(edge_id_t eid, const property& p) {
  edge_info_map::iterator it = local_edges_.find(eid);
  if (it == local_edges_.end()) {
    throw JUBATUS_EXCEPTION(unknown_id("update_edge:eid", eid));
  }
  it->second.p = p;
}

void graph_wo_index::remove_edge(edge_id_t eid) {
  edge_info_map::iterator it = local_edges_.find(eid);
  if (it == local_edges_.end()) {
    throw JUBATUS_EXCEPTION(unknown_id("remove_edge:eid", eid));
  }
  node_id_t src = it->second.src;
  node_id_t tgt = it->second.tgt;

  if (local_nodes_.count(src) > 0) {
    remove_by_swap(local_nodes_[src].out_edges, eid);
  }
  if (local_nodes_.count(tgt) > 0) {
    remove_by_swap(local_nodes_[tgt].in_edges, eid);
  }

  local_edges_.erase(it);
}

void graph_wo_index::add_centrality_query(const preset_query& query) {
  eigen_scores_.insert(make_pair(query, eigen_vector_mixed()));
}

void graph_wo_index::add_shortest_path_query(const preset_query& query) {
  spts_.insert(make_pair(query, spt_mixed()));
}

void graph_wo_index::remove_centrality_query(const preset_query& query) {
  eigen_scores_.erase(query);
}

void graph_wo_index::remove_shortest_path_query(const preset_query& query) {
  spts_.erase(query);
}

double graph_wo_index::centrality(
    node_id_t id,
    centrality_type ct,
    const preset_query& query) const {
  if (ct == EIGENSCORE) {
    eigen_vector_query_mixed::const_iterator model_it
        = eigen_scores_.find(query);
    if (model_it == eigen_scores_.end()) {
      throw JUBATUS_EXCEPTION(unknown_centrality_type(ct));
    }
    eigen_vector_mixed::const_iterator it = model_it->second.find(id);
    if (it == model_it->second.end()) {
      throw JUBATUS_EXCEPTION(unknown_id("centrality", id));
    }
    return it->second.score;
  } else {
    throw JUBATUS_EXCEPTION(unknown_centrality_type(ct));
  }
}

void graph_wo_index::shortest_path(
    node_id_t src,
    node_id_t tgt,
    uint64_t max_hop,
    std::vector<node_id_t>& ret,
    const preset_query& query) const {
  if (global_nodes_.count(src) == 0) {
    throw JUBATUS_EXCEPTION(unknown_id("shortest_path:src", src));
  }
  if (global_nodes_.count(tgt) == 0) {
    throw JUBATUS_EXCEPTION(unknown_id("shortest_path:tgt", tgt));
  }
  spt_query_mixed::const_iterator model_it = spts_.find(query);
  if (model_it == spts_.end()) {
    throw JUBATUS_EXCEPTION(unknown_query(query));
  }
  const spt_mixed& mixed = model_it->second;
  ret.clear();
  uint64_t min_score = ~uint64_t();
  uint64_t ind = ~uint64_t();
  for (uint64_t i = 0; i < mixed.size(); ++i) {
    const shortest_path_tree& spt = mixed[i];
    spt_edges::const_iterator src_it = spt.to_root.find(src);
    spt_edges::const_iterator tgt_it = spt.from_root.find(tgt);
    if (src_it == spt.to_root.end() || tgt_it == spt.from_root.end()) {
      continue;
    }
    uint64_t score = src_it->second.first + tgt_it->second.first;
    if (score < min_score) {
      min_score = score;
      ind = i;
    }
  }

  if (ind >= static_cast<uint64_t>(config_.landmark_num)) {
    return;
  }

  const spt_edges& to_root = mixed[ind].to_root;
  const spt_edges& from_root = mixed[ind].from_root;
  uint64_t landmark = mixed[ind].landmark;

  for (uint64_t cur = src; cur != landmark;) {
    if (ret.size() >= max_hop) {
      return;
    }
    spt_edges::const_iterator it = to_root.find(cur);
    if (it == to_root.end()) {
      ret.clear();
      return;
    }
    ret.push_back(cur);
    if (cur == tgt) {
      return;
    }
    cur = it->second.second;
  }
  ret.push_back(landmark);

  std::vector<node_id_t> from_root_path;
  for (uint64_t cur = tgt; cur != landmark;) {
    if (ret.size() + from_root_path.size() >= max_hop) {
      return;
    }
    spt_edges::const_iterator it = from_root.find(cur);
    if (it == from_root.end()) {
      ret.clear();
      return;
    }
    from_root_path.push_back(cur);
    cur = it->second.second;
  }

  for (vector<node_id_t>::const_reverse_iterator rit = from_root_path.rbegin();
       rit != from_root_path.rend(); ++rit) {
    ret.push_back(*rit);
  }
}

void graph_wo_index::get_node(node_id_t id, node_info& ret) const {
  node_info_map::const_iterator it = local_nodes_.find(id);
  if (it == local_nodes_.end()) {
    throw JUBATUS_EXCEPTION(unknown_id("get_node", id));
  }
  ret = it->second;
}

void graph_wo_index::get_edge(edge_id_t eid, edge_info& ret) const {
  edge_info_map::const_iterator it = local_edges_.find(eid);
  if (it == local_edges_.end()) {
    throw JUBATUS_EXCEPTION(unknown_id("get_edge", eid));
  }
  ret = it->second;
}

string graph_wo_index::type() const {
  return string("graph_wo_index");
}

bool graph_wo_index::save_imp(std::ostream& os) {
  pfi::data::serialization::binary_oarchive oa(os);
  oa << local_nodes_ << local_edges_ << global_nodes_ << eigen_scores_ << spts_;
  return true;
}

bool graph_wo_index::load_imp(std::istream& is) {
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> local_nodes_ >> local_edges_ >> global_nodes_ >> eigen_scores_ >> spts_;
  return true;
}

void graph_wo_index::update_index() {
  update_spt();
}

void graph_wo_index::get_diff_eigen_score(eigen_vector_query_diff& diff) const {
  diff.clear();  // tmp_diff + swap is better ?

  for (eigen_vector_query_mixed::const_iterator query_it
           = eigen_scores_.begin();
       query_it != eigen_scores_.end(); ++query_it) {
    const preset_query& query = query_it->first;
    const eigen_vector_mixed& model = query_it->second;

    double dist = 0;
    for (eigen_vector_mixed::const_iterator it = model.begin();
         it != model.end(); ++it) {
      if (it->second.out_degree_num == 0) {
        dist += it->second.score;
      }
    }

    unordered_set<node_id_t> unmatched_nodes;

    uint64_t new_node_num = 0;
    double dist_from_new_node = 0;
    for (node_info_map::const_iterator node_it = local_nodes_.begin();
         node_it != local_nodes_.end(); ++node_it) {
      if (model.count(node_it->first) == 0) {
        if (!is_matched_to_query(query.node_query, node_it->second.p)) {
          unmatched_nodes.insert(node_it->first);
          continue;
        }
        dist_from_new_node += 1.0;
        ++new_node_num;
      }
    }
    dist += dist_from_new_node;

    if (model.size() + new_node_num > 0) {
      dist /= (model.size() + new_node_num);
    }

    eigen_vector_diff& qdiff = diff[query];

    for (node_info_map::const_iterator node_it = local_nodes_.begin();
         node_it != local_nodes_.end(); ++node_it) {
      if (unmatched_nodes.count(node_it->first)) {
        continue;
      }
      if (!is_matched_to_query(query.node_query, node_it->second.p)) {
        unmatched_nodes.insert(node_it->first);
        continue;
      }

      const std::vector<edge_id_t>& in_edges = node_it->second.in_edges;
      double score = 0;
      for (size_t i = 0; i < in_edges.size(); ++i) {
        if (unmatched_nodes.count(in_edges[i])) {
          continue;
        }
        edge_info_map::const_iterator edge_it = local_edges_.find(in_edges[i]);
        if (edge_it == local_edges_.end()) {
          continue;
        }

        const node_id_t src = edge_it->second.src;
        if (unmatched_nodes.count(src)) {
          continue;
        }
        if (!is_matched_to_query(query.edge_query, edge_it->second.p)) {
          continue;
        }

        eigen_vector_mixed::const_iterator it = model.find(edge_it->second.src);
        if (it == model.end()) {
          continue;
        }
        if (it->second.out_degree_num != 0) {
          // TODO(beam2d) it->second.score > 0 should indicate
          // it->second.out_degree_num
          score += it->second.score / it->second.out_degree_num;
        }
      }

      eigen_vector_info ei;
      ei.score = config_.alpha * score + 1 - config_.alpha
          + config_.alpha * dist;

      if (is_empty_query(query)) {
        ei.out_degree_num = node_it->second.out_edges.size();
      } else {
        uint64_t out_degree = 0;
        for (size_t i = 0; i < node_it->second.out_edges.size(); ++i) {
          const edge_info_map::const_iterator edge_it
              = local_edges_.find(node_it->second.out_edges[i]);
          const edge_info& edge = edge_it->second;
          if (unmatched_nodes.count(edge.tgt)) {
            continue;
          }
          if (!is_node_matched_to_query(query, edge.tgt)) {
            unmatched_nodes.insert(edge.tgt);
            continue;
          }
          if (!is_matched_to_query(query.edge_query, edge.p)) {
            continue;
          }
          ++out_degree;
        }
        ei.out_degree_num = out_degree;
      }

      qdiff[node_it->first] = ei;
    }
  }
}

void graph_wo_index::set_mixed_and_clear_diff_eigen_score(
    eigen_vector_query_mixed& mixed) {
  eigen_scores_ = mixed;
  if (eigen_scores_.size() == 0) {
    return;
  }

  // normalize
  for (eigen_vector_query_mixed::iterator model_it = eigen_scores_.begin();
       model_it != eigen_scores_.end(); ++model_it) {
    normalize(model_it->second);
  }
}

void graph_wo_index::update_spt_edges(
    const preset_query& query,
    spt_edges& se,
    node_id_t landmark,
    bool is_out) {
  se[landmark] = std::make_pair(0, landmark);
  for (node_info_map::const_iterator it = local_nodes_.begin();
       it != local_nodes_.end(); ++it) {
    if (!is_out) {
      update_spt_node(query, it->second.out_edges, se, is_out);
    } else {
      update_spt_node(query, it->second.in_edges, se, is_out);
    }
  }
}

void graph_wo_index::update_spt_node(
    const preset_query& query,
    const std::vector<edge_id_t>& edges,
    spt_edges& se,
    bool is_out) {
  for (size_t i = 0; i < edges.size(); ++i) {
    edge_info& edge = local_edges_[edges[i]];
    const node_id_t from = is_out ? edge.src : edge.tgt;
    const node_id_t to = is_out ? edge.tgt : edge.src;

    if (!is_matched_to_query(query.edge_query, edge.p)
        || !is_node_matched_to_query(query, from)
        || !is_node_matched_to_query(query, to)) {
      continue;
    }

    spt_edges::iterator tr_it = se.find(from);
    if (tr_it != se.end()) {
      uint64_t dist = tr_it->second.first + 1;
      spt_edges::iterator tr_jt = se.find(to);
      if (tr_jt != se.end()) {
        if (dist < tr_jt->second.first) {
          tr_jt->second.first = dist;
          tr_jt->second.second = from;
        }
      } else {
        se.insert(std::make_pair(to, std::make_pair(dist, from)));
      }
    }
  }
}

bool graph_wo_index::is_node_matched_to_query(
    const preset_query& query,
    node_id_t id) const {
  node_info_map::const_iterator it = local_nodes_.find(id);
  if (it == local_nodes_.end()) {
    return true;
  }
  return is_matched_to_query(query.node_query, it->second.p);
}

void graph_wo_index::update_spt() {
  for (spt_query_mixed::iterator it = spts_.begin(); it != spts_.end(); ++it) {
    spt_mixed& mixed = it->second;
    for (size_t i = 0; i < mixed.size(); ++i) {
      shortest_path_tree& spt = mixed[i];
      update_spt_edges(it->first, spt.to_root, spt.landmark, false);
      update_spt_edges(it->first, spt.from_root, spt.landmark, true);
    }
  }
}

void graph_wo_index::get_diff_shortest_path_tree(
    spt_query_diff& all_diff) const {
  all_diff.clear();

  for (spt_query_mixed::const_iterator it = spts_.begin(); it != spts_.end();
       ++it) {
    const spt_mixed& mixed = it->second;
    spt_diff& diff = all_diff[it->first];
    diff.resize(mixed.size());

    for (uint64_t i = 0; i < mixed.size(); ++i) {
      const shortest_path_tree& spt = mixed[i];
      if (spt.landmark == LONG_LONG_MAX) {
        continue;
      }
      diff[i].landmark = spt.landmark;

      for (node_info_map::const_iterator it = local_nodes_.begin();
           it != local_nodes_.end(); ++it) {
        const node_id_t id = it->first;

        spt_edges::const_iterator from_it = spt.from_root.find(id);
        if (from_it != spt.from_root.end()) {
          diff[i].from_root[id] = from_it->second;
        }

        spt_edges::const_iterator to_it = spt.to_root.find(id);
        if (to_it != spt.to_root.end()) {
          diff[i].to_root[id] = to_it->second;
        }
      }
    }
  }
}

void graph_wo_index::set_mixed_and_clear_diff_shortest_path_tree(
    const spt_query_mixed& mixed) {
  spts_ = mixed;
}

void graph_wo_index::get_diff(string& diff) const {
  // get_diff should be constant
  const_cast<graph_wo_index*>(this)->update_spt();

  std::ostringstream os;
  pfi::data::serialization::binary_oarchive oa(os);

  eigen_vector_query_diff ev_diff;
  get_diff_eigen_score(ev_diff);
  oa << ev_diff;

  spt_query_diff sdiff;
  get_diff_shortest_path_tree(sdiff);
  oa << sdiff;
  diff = os.str();
}

void graph_wo_index::set_mixed_and_clear_diff(const string& mixed) {
  std::istringstream is(mixed);
  pfi::data::serialization::binary_iarchive ia(is);

  eigen_vector_query_mixed emixed;
  ia >> emixed;
  set_mixed_and_clear_diff_eigen_score(emixed);

  spt_query_mixed smixed;
  ia >> smixed;
  set_mixed_and_clear_diff_shortest_path_tree(smixed);
}

void graph_wo_index::get_status(map<string, string>& status) const {
  status["local_node_num"] = lexical_cast<string>(local_nodes_.size());
  status["global_node_num"] = lexical_cast<string>(global_nodes_.size());
  status["local_edge_num"] = lexical_cast<string>(local_edges_.size());
}

void graph_wo_index::mix(const string& diff, string& mixed) {
  eigen_vector_query_diff ediff;
  spt_query_diff sdiff;
  {
    std::istringstream is_diff(diff);
    pfi::data::serialization::binary_iarchive ia(is_diff);
    ia >> ediff;
    ia >> sdiff;
  }

  eigen_vector_query_mixed emixed;
  spt_query_mixed smixed;
  {
    std::istringstream is_mixed(mixed);
    pfi::data::serialization::binary_iarchive ia(is_mixed);
    ia >> emixed;
    ia >> smixed;
  }

  mix(ediff, emixed);
  mix(sdiff, smixed, config_.landmark_num);

  std::ostringstream os;
  pfi::data::serialization::binary_oarchive oa(os);

  oa << emixed << smixed;
  mixed = os.str();
}

void graph_wo_index::mix(
    const eigen_vector_query_diff& diff,
    eigen_vector_query_mixed& mixed) {
  for (eigen_vector_query_diff::const_iterator model_it = diff.begin();
      model_it != diff.end(); ++model_it) {
    eigen_vector_mixed& evm = mixed[model_it->first];
    for (eigen_vector_diff::const_iterator it = model_it->second.begin();
         it != model_it->second.end(); ++it) {
      evm[it->first] = it->second;
    }
  }
}

void graph_wo_index::mix_spt(
    const shortest_path_tree& diff,
    shortest_path_tree& mixed) {
  for (spt_edges::const_iterator it = diff.from_root.begin();
       it != diff.from_root.end(); ++it) {
    mixed.from_root[it->first] = it->second;
  }

  for (spt_edges::const_iterator it = diff.to_root.begin();
       it != diff.to_root.end(); ++it) {
    mixed.to_root[it->first] = it->second;
  }
}

void graph_wo_index::mix(
    const spt_query_diff& all_diff,
    spt_query_mixed& all_mixed,
    size_t landmark_num) {
  for (spt_query_diff::const_iterator it = all_diff.begin();
       it != all_diff.end(); ++it) {
    const spt_diff& diff = it->second;
    spt_mixed& mixed = all_mixed[it->first];

    map<node_id_t, uint64_t> diff_landmark2ind;
    for (uint64_t i = 0; i < diff.size(); ++i) {
      if (diff[i].landmark != LONG_LONG_MAX) {
        diff_landmark2ind[diff[i].landmark] = i;
      }
    }

    map<node_id_t, uint64_t> mixed_landmark2ind;
    for (uint64_t i = 0; i < mixed.size(); ++i) {
      if (mixed[i].landmark != LONG_LONG_MAX) {
        mixed_landmark2ind[mixed[i].landmark] = i;
      }
    }

    for (map<node_id_t, uint64_t>::const_iterator it
             = diff_landmark2ind.begin(); it != diff_landmark2ind.end(); ++it) {
      map<node_id_t, uint64_t>::iterator jt
          = mixed_landmark2ind.find(it->first);
      if (jt != mixed_landmark2ind.end()) {
        mix_spt(diff[it->second], mixed[jt->second]);
      } else if (mixed.size() < landmark_num) {
        mixed.push_back(diff[it->second]);
      }
    }
  }
}

void graph_wo_index::remove_by_swap(
    std::vector<edge_id_t>& edges,
    edge_id_t eid) {
  for (size_t i = 0; i < edges.size(); ++i) {
    if (edges[i] == eid) {
      if (i + 1 < edges.size()) {
        swap(edges[i], edges.back());
      }
      edges.resize(edges.size() - 1);
      return;
    }
  }
}

}  // namespace graph
}  // namespace core
}  // namespace jubatus
