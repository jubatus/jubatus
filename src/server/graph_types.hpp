// This file is auto-generated from graph.idl
// *** DO NOT EDIT ***

#ifndef GRAPH_TYPES_HPP_
#define GRAPH_TYPES_HPP_

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <msgpack.hpp>

namespace jubatus {

typedef std::map<std::string, std::string> property;

struct node_info {
 public:

  MSGPACK_DEFINE(p, in_edges, out_edges);

  property p;
  std::vector<uint64_t> in_edges;
  std::vector<uint64_t> out_edges;
};

struct preset_query {
 public:

  MSGPACK_DEFINE(edge_query, node_query);

  std::vector<std::pair<std::string, std::string> > edge_query;
  std::vector<std::pair<std::string, std::string> > node_query;
};

struct edge_info {
 public:

  MSGPACK_DEFINE(p, src, tgt);

  property p;
  std::string src;
  std::string tgt;
};

struct shortest_path_req {
 public:

  MSGPACK_DEFINE(src, tgt, max_hop, q);

  std::string src;
  std::string tgt;
  uint32_t max_hop;
  preset_query q;
};

}  // namespace jubatus

#endif // GRAPH_TYPES_HPP_
