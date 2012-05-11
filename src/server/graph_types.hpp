
// This file is auto-generated from src/server/graph.idl
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

typedef int32_t centrality_type;

typedef std::string node_id;

typedef uint64_t edge_id_t;

typedef std::map<std::string, std::string > property;

struct node_info {
public:

  
  MSGPACK_DEFINE(p, in_edges, out_edges);  

  property p;
  std::vector<edge_id_t > in_edges;
  std::vector<edge_id_t > out_edges;
};

struct edge_info {
public:

  
  MSGPACK_DEFINE(p, src, tgt);  

  property p;
  node_id src;
  node_id tgt;
};

struct edge_req {
public:

  
  MSGPACK_DEFINE(id, info);  

  edge_id_t id;
  edge_info info;
};

struct shortest_path_req {
public:

  
  MSGPACK_DEFINE(src, tgt, max_hop);  

  node_id src;
  node_id tgt;
  size_t max_hop;
};

} // namespace jubatus



#endif // GRAPH_TYPES_HPP_
