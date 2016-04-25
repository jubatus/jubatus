// This file is auto-generated from bandit.idl(0.7.2-79-g2db27d7) with jenerator version 0.8.5-6-g5a2c923/feature/improve-get_labels-ulong
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_BANDIT_TYPES_HPP_
#define JUBATUS_SERVER_SERVER_BANDIT_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "jubatus/core/fv_converter/datum.hpp"
#include <msgpack.hpp>

namespace jubatus {

struct arm_info {
 public:
  MSGPACK_DEFINE(trial_count, weight);
  int32_t trial_count;
  double weight;
  arm_info() {
  }
  arm_info(int32_t trial_count, double weight)
    : trial_count(trial_count), weight(weight) {
  }
};

}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_BANDIT_TYPES_HPP_
