// This file is auto-generated from clustering.idl(0.9.4-19-gc665909) with jenerator version 0.8.5-6-g5a2c923/feature/refactoring_clustering_api
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_CLUSTERING_TYPES_HPP_
#define JUBATUS_CLIENT_CLUSTERING_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <jubatus/client/common/datum.hpp>
#include <msgpack.hpp>

namespace jubatus {
namespace clustering {

struct weighted_datum {
 public:
  MSGPACK_DEFINE(weight, point);
  double weight;
  jubatus::client::common::datum point;
  weighted_datum() {
  }
  weighted_datum(double weight, const jubatus::client::common::datum& point)
    : weight(weight), point(point) {
  }
};

struct indexed_point {
 public:
  MSGPACK_DEFINE(id, point);
  std::string id;
  jubatus::client::common::datum point;
  indexed_point() {
  }
  indexed_point(const std::string& id,
      const jubatus::client::common::datum& point)
    : id(id), point(point) {
  }
};

struct weighted_index {
 public:
  MSGPACK_DEFINE(weight, id);
  double weight;
  std::string id;
  weighted_index() {
  }
  weighted_index(double weight, const std::string& id)
    : weight(weight), id(id) {
  }
};

}  // namespace clustering
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_CLUSTERING_TYPES_HPP_
