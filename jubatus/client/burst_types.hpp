// This file is auto-generated from burst.idl(0.4.5-405-gf5f59de) with jenerator version 0.4.5-418-gd2d5f04/follow-up-0.5.x
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
  MSGPACK_DEFINE(txt, scaling_param, gamma);
  std::string txt;
  double scaling_param;
  double gamma;
  st_keyword() {
  }
  st_keyword(const std::string& txt, double scaling_param, double gamma)
    : txt(txt), scaling_param(scaling_param), gamma(gamma) {
  }
};

struct st_batch {
 public:
  MSGPACK_DEFINE(d, r, batch_weight);
  int32_t d;
  int32_t r;
  double batch_weight;
  st_batch() {
  }
  st_batch(int32_t d, int32_t r, double batch_weight)
    : d(d), r(r), batch_weight(batch_weight) {
  }
};

struct st_window {
 public:
  MSGPACK_DEFINE(start_pos, batches);
  int32_t start_pos;
  std::vector<st_batch> batches;
  st_window() {
  }
  st_window(int32_t start_pos, const std::vector<st_batch>& batches)
    : start_pos(start_pos), batches(batches) {
  }
};

struct st_document {
 public:
  MSGPACK_DEFINE(pos, txt);
  double pos;
  jubatus::client::common::datum txt;
  st_document() {
  }
  st_document(double pos, const jubatus::client::common::datum& txt)
    : pos(pos), txt(txt) {
  }
};

}  // namespace burst
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_BURST_TYPES_HPP_
