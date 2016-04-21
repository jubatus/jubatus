// This file is auto-generated from burst.idl(0.6.4-96-g66ed74d) with jenerator version 0.8.5-6-g5a2c923/feature/improve-get_labels-ulong
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_SERVER_BURST_TYPES_HPP_
#define JUBATUS_SERVER_SERVER_BURST_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "jubatus/core/fv_converter/datum.hpp"
#include <msgpack.hpp>

namespace jubatus {

struct keyword_with_params {
 public:
  MSGPACK_DEFINE(keyword, scaling_param, gamma);
  std::string keyword;
  double scaling_param;
  double gamma;
  keyword_with_params() {
  }
  keyword_with_params(const std::string& keyword, double scaling_param,
      double gamma)
    : keyword(keyword), scaling_param(scaling_param), gamma(gamma) {
  }
};

struct batch {
 public:
  MSGPACK_DEFINE(all_data_count, relevant_data_count, burst_weight);
  int32_t all_data_count;
  int32_t relevant_data_count;
  double burst_weight;
  batch() {
  }
  batch(int32_t all_data_count, int32_t relevant_data_count,
      double burst_weight)
    : all_data_count(all_data_count), relevant_data_count(relevant_data_count),
        burst_weight(burst_weight) {
  }
};

struct window {
 public:
  MSGPACK_DEFINE(start_pos, batches);
  double start_pos;
  std::vector<batch> batches;
  window() {
  }
  window(double start_pos, const std::vector<batch>& batches)
    : start_pos(start_pos), batches(batches) {
  }
};

struct document {
 public:
  MSGPACK_DEFINE(pos, text);
  double pos;
  std::string text;
  document() {
  }
  document(double pos, const std::string& text)
    : pos(pos), text(text) {
  }
};

}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_BURST_TYPES_HPP_
