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

#include "graph.hpp"

#include <string>
#include <utility>
#include <vector>

#include <pficommon/lang/shared_ptr.h>

#include "../graph/graph_factory.hpp"
#include "../common/vector_util.hpp"
#include "../storage/storage_factory.hpp"

using std::string;
using std::vector;
using std::pair;
using jubatus::core::framework::mixable_holder;
using jubatus::core::graph::node_id_t;
using jubatus::core::graph::edge_id_t;
using jubatus::core::graph::preset_query;
using jubatus::core::graph::property;
using pfi::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace driver {

graph::graph(jubatus::core::graph::graph_base* graph_method)
    : mixable_holder_(new mixable_holder) {
  pfi::lang::shared_ptr<jubatus::core::graph::graph_base>
      graph_method_p(graph_method);
  graph_.set_model(graph_method_p);

  mixable_holder_->register_mixable(&graph_);
}

graph::~graph() {
}

void graph::create_node(node_id_t id) {  /* no lock here */
  this->create_node_here(id);
}

void graph::update_node(node_id_t id, const property& p) {
  graph_.get_model()->update_node(id, p);
}

void graph::remove_node(node_id_t id) {
  graph_.get_model()->remove_node(id);
  graph_.get_model()->remove_global_node(id);
}

void graph::create_edge(
    edge_id_t eid,
    node_id_t src,
    node_id_t target,
    const property & p) {
  // TODO(kuenishi): assert id==ei.src
  this->create_edge_here(eid, src, target, p);
}

void graph::update_edge(edge_id_t eid, const property& p) {
  graph_.get_model()->update_edge(eid, p);
}

void graph::remove_edge(edge_id_t eid) {
  graph_.get_model()->remove_edge(eid);
}

double graph::get_centrality(
    node_id_t nid,
    jubatus::core::graph::centrality_type ct,
    const preset_query& q) const {
  return graph_.get_model()->centrality(nid, ct, q);
}

std::vector<node_id_t> graph::get_shortest_path(
    node_id_t src,
    node_id_t target,
    uint64_t max_hop,
    const preset_query &q) const {
  std::vector<jubatus::core::graph::node_id_t> ret;
  graph_.get_model()->shortest_path(src, target, max_hop, ret, q);
  return ret;
}

void graph::add_centrality_query(const preset_query& q) {
  graph_.get_model()->add_centrality_query(q);
}

void graph::add_shortest_path_query(const preset_query& q) {
  graph_.get_model()->add_shortest_path_query(q);
}

void graph::remove_centrality_query(const preset_query& q) {
  graph_.get_model()->remove_centrality_query(q);
}

void graph::remove_shortest_path_query(const preset_query& q) {
  graph_.get_model()->remove_shortest_path_query(q);
}

jubatus::core::graph::node_info graph::get_node(node_id_t nid) const {
  jubatus::core::graph::node_info info;
  graph_.get_model()->get_node(nid, info);
  return info;
}

jubatus::core::graph::edge_info graph::get_edge(edge_id_t eid) const {
  jubatus::core::graph::edge_info info;
  graph_.get_model()->get_edge(eid, info);
  return info;
}

void graph::update_index() {
  graph_.get_model()->update_index();
  std::string diff;
  graph_.get_model()->get_diff(diff);
  graph_.get_model()->set_mixed_and_clear_diff(diff);
}

void graph::clear() {
  graph_.get_model()->clear();
}

void graph::create_node_here(node_id_t nid) {
  try {
    graph_.get_model()->create_node(nid);
    graph_.get_model()->create_global_node(nid);
  } catch (const jubatus::core::graph::local_node_exists& e) {  // pass through
  } catch (const jubatus::core::graph::global_node_exists& e) {  // pass through
  } catch (const std::runtime_error& e) {
    throw;
  }
}

void graph::remove_global_node(node_id_t nid) {
  try {
    graph_.get_model()->remove_global_node(nid);
  } catch (const jubatus::core::graph::local_node_exists& e) {
  } catch (const jubatus::core::graph::global_node_exists& e) {
  } catch (const std::runtime_error& e) {
    throw;
  }
}  // update internal

void graph::create_edge_here(
    edge_id_t eid,
    node_id_t src,
    node_id_t target,
    const property& p) {
  try {
    graph_.get_model()->create_edge(eid, src, target);
    graph_.get_model()->update_edge(eid, p);
  } catch (const jubatus::core::graph::graph_exception& e) {
    throw;
  }
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
