// This file is auto-generated from graph.idl(0.5.4-179-gb59b61b) with jenerator version 0.5.4-224-g49229fa/develop
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_GRAPH_TYPES_HPP_
#define JUBATUS_SERVER_SERVER_GRAPH_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "jubatus/core/fv_converter/datum.hpp"
#include <msgpack.hpp>

#include "jubatus/core/graph/graph_type.hpp"

namespace jubatus {

struct query {
 public:
  MSGPACK_DEFINE(from_id, to_id);
  std::string from_id;
  std::string to_id;
  query() {
  }
  query(const std::string& from_id, const std::string& to_id)
    : from_id(from_id), to_id(to_id) {
  }
};

struct edge {
 public:
  MSGPACK_DEFINE(property, source, target);
  std::map<std::string, std::string> property;
  std::string source;
  std::string target;
  edge() {
  }
  edge(const std::map<std::string, std::string>& property,
      const std::string& source, const std::string& target)
    : property(property), source(source), target(target) {
  }
};

struct shortest_path_query {
 public:
  MSGPACK_DEFINE(source, target, max_hop, query);
  std::string source;
  std::string target;
  uint32_t max_hop;
  jubatus::core::graph::preset_query query;
  shortest_path_query() {
  }
  shortest_path_query(const std::string& source, const std::string& target,
      uint32_t max_hop, const jubatus::core::graph::preset_query& query)
    : source(source), target(target), max_hop(max_hop), query(query) {
  }
};

}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_GRAPH_TYPES_HPP_
