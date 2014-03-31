// This file is auto-generated from anomaly.idl(0.4.5-347-g86989a6) with jenerator version 0.5.2-17-g8a5dca4/develop
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_ANOMALY_TYPES_HPP_
#define JUBATUS_CLIENT_ANOMALY_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <jubatus/client/common/datum.hpp>
#include <msgpack.hpp>

namespace jubatus {
namespace anomaly {

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

}  // namespace anomaly
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_ANOMALY_TYPES_HPP_
