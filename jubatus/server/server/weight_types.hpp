// This file is auto-generated from weight.idl(0.9.0-24-gda61383) with jenerator version 0.9.4-42-g70f7539/develop
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_WEIGHT_TYPES_HPP_
#define JUBATUS_SERVER_SERVER_WEIGHT_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "jubatus/core/fv_converter/datum.hpp"
#include <msgpack.hpp>

namespace jubatus {

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

}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_WEIGHT_TYPES_HPP_
