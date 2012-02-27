#pragma once

#include <map>
#include <string>
#include <pficommon/data/serialization.h>
#include <pficommon/data/unordered_map.h>
#include "../common/type.hpp"

namespace jubatus{

typedef uint64_t node_id_t;
typedef uint64_t edge_id_t;

typedef std::map<std::string, std::string> property;

enum centerality_type {
  EIGENSCORE = 0
};

struct eigen_vector_info{
  double score;
  uint64_t out_degree_num;

  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(score)
      & MEMBER(out_degree_num);
  }

};

typedef pfi::data::unordered_map<node_id_t, eigen_vector_info> eigen_vector_diff;
typedef pfi::data::unordered_map<node_id_t, eigen_vector_info> eigen_vector_mixed;

struct node_info{
  property p;
  std::vector<edge_id_t> in_edges;
  std::vector<edge_id_t> out_edges;

  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(p)
      & MEMBER(in_edges)
      & MEMBER(out_edges);
  }

};

struct edge_info{
  property p;
  node_id_t src;
  node_id_t tgt;

  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(p)
      & MEMBER(src)
      & MEMBER(tgt);
  }

};

const size_t LANDMARK_NUM = 5;

typedef pfi::data::unordered_map<node_id_t, std::pair<uint64_t, node_id_t> > spt_edges;

struct shortest_path_tree{
  node_id_t landmark;
  spt_edges from_root;
  spt_edges to_root;

  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar){
    ar & MEMBER(landmark)
      & MEMBER(from_root)
      & MEMBER(to_root);
  }
};

typedef std::vector<shortest_path_tree> spt_mixed;
typedef std::vector<shortest_path_tree> spt_diff;

}
