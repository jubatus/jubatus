// This file is auto-generated from bandit.idl(0.6.4-14-g3b68062) with jenerator version 0.5.4-224-g49229fa/feature/bandit
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_BANDIT_TYPES_HPP_
#define JUBATUS_CLIENT_BANDIT_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <jubatus/client/common/datum.hpp>
#include <msgpack.hpp>

namespace jubatus {
namespace bandit {

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

}  // namespace bandit
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_BANDIT_TYPES_HPP_
