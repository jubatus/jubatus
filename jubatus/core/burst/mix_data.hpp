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

#ifndef JUBATUS_CORE_BURST_MIX_DATA_HPP_
#define JUBATUS_CORE_BURST_MIX_DATA_HPP_

#include <string>
#include <msgpack.hpp>

#include "window/window.hpp"
#include "jubatus/util/data/serialization.h"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

class mix_data {
 public:
  mix_data() {
  }

  /**
   * constructor.
   * @param w window
   * @param scaling_param
   *        Kleinberg paper's "scaling parameter"
   * @param gamma
   *        Kleinberg paper's "gamma"
   */
  mix_data(const window& w,
           double scaling_param,
           double gamma);

  window get_window() const {
    return w_;
  }

  double get_scaling_param() const {
    return scaling_param_;
  }

  double get_gamma() const {
    return gamma_;
  }

  MSGPACK_DEFINE(w_, scaling_param_, gamma_);

 private:
  window w_;
  double scaling_param_;
  double gamma_;

  template<class Archiver>
  void serialize(Archiver& ar) {
    ar & JUBA_MEMBER(w_) & JUBA_MEMBER(scaling_param_) & JUBA_MEMBER(gamma_);
  }
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_MIX_DATA_HPP_
