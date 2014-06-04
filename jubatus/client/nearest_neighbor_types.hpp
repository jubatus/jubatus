// This file is auto-generated from nearest_neighbor.idl(0.4.5-347-g86989a6) with jenerator version 0.5.2-45-gc4cfc98/develop
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_NEAREST_NEIGHBOR_TYPES_HPP_
#define JUBATUS_CLIENT_NEAREST_NEIGHBOR_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <jubatus/client/common/datum.hpp>
#include <msgpack.hpp>

namespace jubatus {
namespace nearest_neighbor {

struct id_with_score {
 public:
  MSGPACK_DEFINE(id, score);
  std::string id;
  float score;
  id_with_score() {
  }
  id_with_score(const std::string& id, float score)
    : id(id), score(score) {
  }
};

}  // namespace nearest_neighbor
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_NEAREST_NEIGHBOR_TYPES_HPP_
