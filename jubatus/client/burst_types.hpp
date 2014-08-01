// This file is auto-generated from burst.idl(0.6.1-19-gfaa73d5) with jenerator version 0.5.4-224-g49229fa/feature/burst
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_BURST_TYPES_HPP_
#define JUBATUS_CLIENT_BURST_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <jubatus/client/common/datum.hpp>
#include <msgpack.hpp>

namespace jubatus {
namespace burst {

struct st_keyword {
 public:
  MSGPACK_DEFINE(keyword, scaling_param, gamma);
  std::string keyword;
  double scaling_param;
  double gamma;
  st_keyword() {
  }
  st_keyword(const std::string& keyword, double scaling_param, double gamma)
    : keyword(keyword), scaling_param(scaling_param), gamma(gamma) {
  }
};

struct st_batch {
 public:
  MSGPACK_DEFINE(d, r, burst_weight);
  int32_t d;
  int32_t r;
  double burst_weight;
  st_batch() {
  }
  st_batch(int32_t d, int32_t r, double burst_weight)
    : d(d), r(r), burst_weight(burst_weight) {
  }
};

struct st_window {
 public:
  MSGPACK_DEFINE(start_pos, batches);
  double start_pos;
  std::vector<st_batch> batches;
  st_window() {
  }
  st_window(double start_pos, const std::vector<st_batch>& batches)
    : start_pos(start_pos), batches(batches) {
  }
};

struct st_document {
 public:
  MSGPACK_DEFINE(pos, text);
  double pos;
  std::string text;
  st_document() {
  }
  st_document(double pos, const std::string& text)
    : pos(pos), text(text) {
  }
};

}  // namespace burst
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_BURST_TYPES_HPP_
