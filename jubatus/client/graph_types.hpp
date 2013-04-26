// This file is auto-generated from graph.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_GRAPH_TYPES_HPP_
#define JUBATUS_CLIENT_GRAPH_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <msgpack.hpp>

namespace jubatus {
namespace graph {

struct node {
 public:
  MSGPACK_DEFINE(property, in_edges, out_edges);
  std::map<std::string, std::string> property;
  std::vector<uint64_t> in_edges;
  std::vector<uint64_t> out_edges;
};

struct preset_query {
 public:
  MSGPACK_DEFINE(edge_query, node_query);
  std::vector<std::pair<std::string, std::string> > edge_query;
  std::vector<std::pair<std::string, std::string> > node_query;
};

struct edge {
 public:
  MSGPACK_DEFINE(property, source, target);
  std::map<std::string, std::string> property;
  std::string source;
  std::string target;
};

struct shortest_path_query {
 public:
  MSGPACK_DEFINE(source, target, max_hop, query);
  std::string source;
  std::string target;
  uint32_t max_hop;
  preset_query query;
};

}  // namespace graph
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_GRAPH_TYPES_HPP_
