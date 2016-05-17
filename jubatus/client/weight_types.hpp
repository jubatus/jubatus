// This file is auto-generated from weight.idl with jenerator version 0.8.5-6-g5a2c923/develop
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_WEIGHT_TYPES_HPP_
#define JUBATUS_CLIENT_WEIGHT_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <jubatus/client/common/datum.hpp>
#include <msgpack.hpp>

namespace jubatus {
namespace weight {

struct feature {
 public:
  MSGPACK_DEFINE(key, value);
  std::string key;
  float value;
  feature() {
  }
  feature(const std::string& key, float value)
    : key(key), value(value) {
  }
};

}  // namespace weight
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_WEIGHT_TYPES_HPP_
