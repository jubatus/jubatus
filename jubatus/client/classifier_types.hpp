// This file is auto-generated from classifier.idl(0.8.9-17-gd4c007f) with jenerator version 0.8.5-6-g5a2c923/feature/improve-get_labels-ulong
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_CLASSIFIER_TYPES_HPP_
#define JUBATUS_CLIENT_CLASSIFIER_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <jubatus/client/common/datum.hpp>
#include <msgpack.hpp>

namespace jubatus {
namespace classifier {

struct estimate_result {
 public:
  MSGPACK_DEFINE(label, score);
  std::string label;
  double score;
  estimate_result() {
  }
  estimate_result(const std::string& label, double score)
    : label(label), score(score) {
  }
};

struct labeled_datum {
 public:
  MSGPACK_DEFINE(label, data);
  std::string label;
  jubatus::client::common::datum data;
  labeled_datum() {
  }
  labeled_datum(const std::string& label,
      const jubatus::client::common::datum& data)
    : label(label), data(data) {
  }
};

}  // namespace classifier
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_CLASSIFIER_TYPES_HPP_
