// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

/**
 * @file batch.hpp
 */
#ifndef JUBATUS_CORE_BURST_WINDOW_BATCH_HPP_
#define JUBATUS_CORE_BURST_WINDOW_BATCH_HPP_

#include <stdint.h>

#include <msgpack.hpp>
#include "jubatus/util/data/serialization.h"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

static const double kDefaultBatchWeight = -1;

class batch {
 public:
  batch();

  bool add_document(const uint32_t d, const uint32_t r);

  uint32_t get_d() const {
    return d_;
  }

  uint32_t get_r() const {
    return r_;
  }

  double get_batch_weight() const {
    return batch_weight_;
  }

  void set_batch_weight(const double& batch_weight) {
    batch_weight_ = batch_weight;
  }

  /**
   * copy d & r.
   * @param b the source object
   */
  void copy_d_r(const batch & b);

  /**
   * copy batch_weight.
   * @param b the source object
   */
  void copy_batch_weight(const batch& b);

  MSGPACK_DEFINE(d_, r_, batch_weight_);

 private:
  int32_t d_;  // all data count
  int32_t r_;  // relevant data count
  double batch_weight_;

  template<class Archiver>
  void serialize(Archiver& ar) {
    ar & JUBA_MEMBER(d_) & JUBA_MEMBER(r_) & JUBA_MEMBER(batch_weight_);
  }
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_WINDOW_BATCH_HPP_
