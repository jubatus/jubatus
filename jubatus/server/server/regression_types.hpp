// This file is auto-generated from regression.idl(0.5.2-68-g68e898d) with jenerator version 0.5.4-224-g49229fa/develop
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_REGRESSION_TYPES_HPP_
#define JUBATUS_SERVER_SERVER_REGRESSION_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "jubatus/core/fv_converter/datum.hpp"
#include <msgpack.hpp>

namespace jubatus {

struct scored_datum {
 public:
  MSGPACK_DEFINE(score, data);
  float score;
  jubatus::core::fv_converter::datum data;
  scored_datum() {
  }
  scored_datum(float score, const jubatus::core::fv_converter::datum& data)
    : score(score), data(data) {
  }
};

}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_REGRESSION_TYPES_HPP_
