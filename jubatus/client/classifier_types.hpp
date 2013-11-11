// This file is auto-generated from classifier.idl(0.4.5-331-gd7b15b4) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
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
