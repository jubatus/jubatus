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

#ifndef JUBATUS_CORE_BURST_ENGINE_BURST_DETECT_HPP_
#define JUBATUS_CORE_BURST_ENGINE_BURST_DETECT_HPP_

#include <stdint.h>
#include <vector>

namespace jubatus {
namespace core {
namespace burst {
namespace engine {

void burst_detect(const std::vector<uint32_t>& d_vector,
                  const std::vector<uint32_t>& r_vector,
                  std::vector<double>& batch_weights,
                  double scaling_param,
                  double gamma,
                  double burst_cut_threshold);

}  // namespace engine
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_ENGINE_BURST_DETECT_HPP_
