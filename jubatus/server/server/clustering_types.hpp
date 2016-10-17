// This file is auto-generated from clustering.idl with jenerator version 0.8.5-6-g5a2c923/master
// *** DO NOT EDIT ***

#ifndef CLUSTERING_TYPES_HPP_
#define CLUSTERING_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <jubatus/core/fv_converter/datum.hpp>
#include <msgpack.hpp>

#include "jubatus/core/fv_converter/datum.hpp"

namespace jubatus {

struct indexed_point {
 public:
  MSGPACK_DEFINE(id, point);
  std::string id;
  jubatus::core::fv_converter::datum point;
  indexed_point() {
  }
  indexed_point(const std::string& id,
      const jubatus::core::fv_converter::datum& point)
    : id(id), point(point) {
  }
};

}  // namespace jubatus

#endif  // CLUSTERING_TYPES_HPP_
