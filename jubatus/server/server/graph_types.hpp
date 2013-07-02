// This file is auto-generated from graph.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_GRAPH_TYPES_HPP_
#define JUBATUS_SERVER_SERVER_GRAPH_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <msgpack.hpp>

#include "../../core/graph/graph_type.hpp"

namespace jubatus {

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
  jubatus::core::graph::preset_query query;
};

}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_GRAPH_TYPES_HPP_
