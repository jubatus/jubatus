// This file is auto-generated from window_classifier.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_WINDOW_CLASSIFIER_TYPES_HPP_
#define JUBATUS_SERVER_WINDOW_CLASSIFIER_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <msgpack.hpp>

namespace jubatus {

struct datum {
 public:
  MSGPACK_DEFINE(string_values, num_values);
  std::vector<std::pair<std::string, std::string> > string_values;
  std::vector<std::pair<std::string, double> > num_values;
};

struct estimate_result {
 public:
  MSGPACK_DEFINE(label, prob);
  std::string label;
  double prob;
};

}  // namespace jubatus

#endif  // JUBATUS_SERVER_WINDOW_CLASSIFIER_TYPES_HPP_
