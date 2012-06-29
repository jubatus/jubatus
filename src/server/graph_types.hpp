
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

struct preset_query {
public:

  
  MSGPACK_DEFINE(edge_query, node_query);  

  std::vector<std::pair<std::string, std::string > > edge_query;
  std::vector<std::pair<std::string, std::string > > node_query;
};

struct edge_info {
public:

  
  MSGPACK_DEFINE(p, src, tgt);  

  property p;
  node_id src;
  node_id tgt;
};

struct shortest_path_req {
public:

  
  MSGPACK_DEFINE(src, tgt, max_hop, q);  

  node_id src;
  node_id tgt;
  uint32_t max_hop;
  preset_query q;
};

} // namespace jubatus



#endif // GRAPH_TYPES_HPP_
