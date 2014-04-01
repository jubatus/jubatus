// This file is auto-generated from clustering.idl(0.4.5-350-g9c67807) with jenerator version 0.5.2-45-gc4cfc98/develop
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

}  // namespace clustering
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_CLUSTERING_TYPES_HPP_
