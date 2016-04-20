// This file is auto-generated from anomaly.idl(0.7.2-50-gbcc1e21) with jenerator version 0.8.5-6-g5a2c923/feature/improve-get_labels-ulong
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_ANOMALY_TYPES_HPP_
#define JUBATUS_SERVER_SERVER_ANOMALY_TYPES_HPP_

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

#endif  // JUBATUS_SERVER_SERVER_ANOMALY_TYPES_HPP_
