// This file is auto-generated from regression.idl(0.4.5-347-g86989a6) with jenerator version 0.5.2-17-g8a5dca4/develop
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_REGRESSION_TYPES_HPP_
#define JUBATUS_CLIENT_REGRESSION_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <jubatus/client/common/datum.hpp>
#include <msgpack.hpp>

namespace jubatus {
namespace regression {

struct scored_datum {
 public:
  MSGPACK_DEFINE(score, data);
  float score;
  jubatus::client::common::datum data;
  scored_datum() {
  }
  scored_datum(float score, const jubatus::client::common::datum& data)
    : score(score), data(data) {
  }
};

}  // namespace regression
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_REGRESSION_TYPES_HPP_
