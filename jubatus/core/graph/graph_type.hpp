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

#ifndef JUBATUS_CORE_GRAPH_GRAPH_TYPE_HPP_
#define JUBATUS_CORE_GRAPH_GRAPH_TYPE_HPP_

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <msgpack.hpp>
#include <pficommon/data/serialization.h>
#include <pficommon/data/unordered_map.h>

#include "../common/exception.hpp"
#include "../common/type.hpp"

namespace jubatus {
namespace core {
namespace graph {

typedef uint64_t node_id_t;
typedef uint64_t edge_id_t;

typedef std::map<std::string, std::string> property;

enum centrality_type {
  EIGENSCORE = 0
};

struct eigen_vector_info {
  double score;
  uint64_t out_degree_num;

  friend class pfi::data::serialization::access;

  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(score) & MEMBER(out_degree_num);
  }
};

typedef pfi::data::unordered_map<node_id_t, eigen_vector_info>
  eigen_vector_diff;
typedef pfi::data::unordered_map<node_id_t, eigen_vector_info>
  eigen_vector_mixed;

struct node_info {
  property p;
  std::vector<edge_id_t> in_edges;
  std::vector<edge_id_t> out_edges;

  MSGPACK_DEFINE(p, in_edges, out_edges);

  friend class pfi::data::serialization::access;

  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(p) & MEMBER(in_edges) & MEMBER(out_edges);
  }
};

struct edge_info {
  property p;
  node_id_t src;
  node_id_t tgt;

  MSGPACK_DEFINE(p, src, tgt);

  friend class pfi::data::serialization::access;

  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(p) & MEMBER(src) & MEMBER(tgt);
  }
};

struct preset_query {
  // all AND conditions
  std::vector<std::pair<std::string, std::string> > edge_query;
  std::vector<std::pair<std::string, std::string> > node_query;

  bool operator==(const preset_query& r) const {
    return edge_query == r.edge_query && node_query == r.node_query;
  }

  MSGPACK_DEFINE(edge_query, node_query);

  friend class pfi::data::serialization::access;

  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(edge_query) & MEMBER(node_query);
  }
};

typedef pfi::data::unordered_map<node_id_t, std::pair<uint64_t, node_id_t> >
  spt_edges;

struct shortest_path_tree {
  node_id_t landmark;
  spt_edges from_root;
  spt_edges to_root;

  friend class pfi::data::serialization::access;

  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(landmark) & MEMBER(from_root) & MEMBER(to_root);
  }
};

typedef std::vector<shortest_path_tree> spt_mixed;
typedef std::vector<shortest_path_tree> spt_diff;

typedef pfi::data::unordered_map<preset_query, eigen_vector_mixed>
  eigen_vector_query_mixed;
typedef pfi::data::unordered_map<preset_query, eigen_vector_diff>
  eigen_vector_query_diff;

typedef pfi::data::unordered_map<preset_query, spt_mixed> spt_query_mixed;
typedef pfi::data::unordered_map<preset_query, spt_diff> spt_query_diff;

class graph_exception : public jubatus::exception::runtime_error {
 public:
  explicit graph_exception(const std::string& what)
      : runtime_error(what) {
  }
};

class unknown_graph : public graph_exception {
 public:
  explicit unknown_graph(const std::string& name)
      : graph_exception(name) {
  }
};

class local_node_exists : public graph_exception {
 public:
  explicit local_node_exists(node_id_t id)
      : graph_exception(__func__),
        id_(id) {
  }

  node_id_t id_;
};

class global_node_exists : public graph_exception {
 public:
  explicit global_node_exists(node_id_t id)
      : graph_exception(__func__),
        id_(id) {
  }

  node_id_t id_;
};

class edge_exists : public graph_exception {
 public:
  explicit edge_exists(edge_id_t id)
      : graph_exception(__func__),
        id_(id) {
  }

  edge_id_t id_;
};

class unknown_id : public graph_exception {
 public:
  explicit unknown_id(const std::string& type, uint64_t id)
      : graph_exception(type),
        id_(id) {
  }

  uint64_t id_;
};

class unknown_centrality_type : public graph_exception {
 public:
  explicit unknown_centrality_type(centrality_type t)
      : graph_exception(__func__),
        t_(t) {
  }

  centrality_type t_;
};

class unknown_query : public graph_exception {
 public:
  explicit unknown_query(const preset_query& q)
      : graph_exception(__func__),
        q_(q) {
  }

  virtual ~unknown_query() throw() {
  }  // mmm...

  preset_query q_;
};

}  // namespace graph
}  // namespace core
}  // namespace jubatus

namespace pfi {
namespace data {

template<> struct hash<jubatus::core::graph::preset_query> {
  uint64_t operator()(const jubatus::core::graph::preset_query& p) const {
    return update(p.node_query, update(p.edge_query, 14695981039346656037LLU));
  }

 private:
  static uint64_t update(
      const std::vector<std::pair<std::string, std::string> >& q,
      uint64_t h) {
    for (size_t i = 0; i < q.size(); ++i) {
      h = update(q[i].first, h);
      h = update(q[i].second, h);
    }
    return h;
  }

  static uint64_t update(const std::string& s, uint64_t h) {
    for (size_t i = 0; i < s.size(); ++i) {
      h *= 1099511628211LLU;
      h ^= s[i];
    }
    return h * 1099511628211LLU;
  }
};

}  // namespace data
}  // namespace pfi

#endif  // JUBATUS_CORE_GRAPH_GRAPH_TYPE_HPP_
