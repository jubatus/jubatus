// This file is auto-generated from bandit.idl(0.6.4-37-g8b6a586) with jenerator version 0.6.4-104-g3698e11/gintenlabo-fix-956
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
