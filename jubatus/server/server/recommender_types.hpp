// This file is auto-generated from recommender.idl(0.4.5-350-g9c67807) with jenerator version 0.4.5-375-g07d06b7/develop
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_RECOMMENDER_TYPES_HPP_
#define JUBATUS_SERVER_SERVER_RECOMMENDER_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "jubatus/core/fv_converter/datum.hpp"
#include <msgpack.hpp>

namespace jubatus {

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

}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_RECOMMENDER_TYPES_HPP_
