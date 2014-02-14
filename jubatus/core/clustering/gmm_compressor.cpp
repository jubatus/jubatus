// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "gmm_compressor.hpp"

#include <cmath>

namespace jubatus {
namespace core {
namespace clustering {
namespace compressor {

double gmm_compressor::get_probability(
    const weighted_point& p,
    double min_dist,
    const weighted_point& bp,
    double bp_score,
    double weight_sum,
    double squared_min_dist_sum) {
  return std::ceil(weight_sum * (
      5.0 / bp_score
      + min_dist * min_dist * p.weight / squared_min_dist_sum));
}

}  // namespace compressor
}  // namespace clustering
}  // namespace core
}  // namespace jubatus
